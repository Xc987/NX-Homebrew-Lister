#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "read.h"

bool fileExists(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

void removeCR(char* json) {
    char* src = json;
    char* dest = json;
    while (*src) {
        if (*src != '\r') {
            *dest = *src;
            dest++;
        }
        src++;
    }
    *dest = '\0';
}
char* extractValueForKey(const char* json, const char* key) {
    char* keyPattern = (char*)malloc(strlen(key) + 3);
    sprintf(keyPattern, "\"%s\"", key);
    char* keyPos = strstr(json, keyPattern);
    free(keyPattern);
    if (!keyPos) {
        return NULL;
    }
    char* colonPos = strchr(keyPos, ':');
    if (!colonPos) {
        return NULL;
    }
    char* valueStart = colonPos + 1;
    while (*valueStart == ' ' || *valueStart == '\t' || *valueStart == '\n' || *valueStart == '\r') {
        valueStart++;
    }
    if (*valueStart == '"') {
        valueStart++;
        char* valueEnd = strchr(valueStart, '"');
        if (!valueEnd) {
            return NULL;
        }
        size_t valueLength = valueEnd - valueStart;
        char* value = (char*)malloc(valueLength + 1);
        strncpy(value, valueStart, valueLength);
        value[valueLength] = '\0';
        return value;
    } else if (*valueStart == 't' || *valueStart == 'f') {
        char* valueEnd = strpbrk(valueStart, ",}\n");
        if (!valueEnd) {
            valueEnd = valueStart + strlen(valueStart);
        }
        size_t valueLength = valueEnd - valueStart;
        char* value = (char*)malloc(valueLength + 1);
        strncpy(value, valueStart, valueLength);
        value[valueLength] = '\0';
        if (strcmp(value, "true") == 0) {
            free(value);
            return strdup("true");
        } else if (strcmp(value, "false") == 0) {
            free(value);
            return strdup("false");
        }
    } else {
        char* valueEnd = strpbrk(valueStart, ",}\n");
        if (!valueEnd) {
            valueEnd = valueStart + strlen(valueStart);
        }
        size_t valueLength = valueEnd - valueStart;
        char* value = (char*)malloc(valueLength + 1);
        strncpy(value, valueStart, valueLength);
        value[valueLength] = '\0';
        return value;
    }
    return NULL;
}
void loadAsset(Asset *asset, uint8_t *data, size_t size) {
    size_t offset = 0x18;
    uint64_t nacp_pos = *(uint64_t *)(data + offset);
    uint64_t nacp_size = *(uint64_t *)(data + offset + 0x8);
    asset->nacp = (uint8_t *)malloc(nacp_size);
    memcpy(asset->nacp, data + nacp_pos, nacp_size);
    offset += 0x10;
    uint64_t romfs_pos = *(uint64_t *)(data + offset);
    uint64_t romfs_size = *(uint64_t *)(data + offset + 0x8);
    asset->romfs = (uint8_t *)malloc(romfs_size);
    memcpy(asset->romfs, data + romfs_pos, romfs_size);
    strncpy(asset->name, (char *)(asset->nacp), 512);
    asset->name[511] = '\0';
    strncpy(asset->author, (char *)(asset->nacp + 0x200), 256);
    asset->author[255] = '\0';
    strncpy(asset->version, (char *)(asset->nacp + 0x3060), 16);
    asset->version[15] = '\0';
}

int loadBinaryData(Editor *editor) {
    FILE *file = fopen(editor->filename, "rb");
    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    editor->data = (uint8_t *)malloc(filesize);
    fread(editor->data, 1, filesize, file);
    fclose(file);
    uint8_t *data = editor->data;
    if (memcmp(data + 0x10, NRO_MAGIC, 4) != 0) {
        fprintf(stderr, "Invalid file format!\n");
        free(editor->data);
        return 0;
    }
    editor->nrosize = *(uint32_t *)(data + 0x18);
    if (filesize > editor->nrosize + 4 && memcmp(data + editor->nrosize, ASET_MAGIC, 4) == 0) {
        loadAsset(&editor->asset, data + editor->nrosize, filesize - editor->nrosize);
    }
    return 1;
}
int checkStarFile(const char *dirpath, const char *filename) {
    char starFilePath[1024];
    snprintf(starFilePath, sizeof(starFilePath), "%s/.%s.star", dirpath, filename);
    FILE *file = fopen(starFilePath, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}
double getFileSize(const char *file_path) {
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        return 0;
    }
    double size_in_mb = (double)file_stat.st_size / (1024 * 1024);
    return size_in_mb;
}
long getDirectorySize(const char* path) {
    DIR* dir;
    struct dirent* entry;
    struct stat fileStat;
    long totalSize = 0;

    dir = opendir(path);
    if (!dir) {
        printf("Failed to open directory: %s\n", path);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char fullPath[PATH_MAX];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
        
        if (stat(fullPath, &fileStat) == 0) {
            if (S_ISDIR(fileStat.st_mode)) {
                totalSize += getDirectorySize(fullPath);
            } else {
                totalSize += fileStat.st_size;
            }
        }
    }

    closedir(dir);
    return totalSize;
}