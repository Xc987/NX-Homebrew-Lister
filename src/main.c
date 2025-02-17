#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define NRO_MAGIC "NRO0"
#define ASET_MAGIC "ASET"

typedef struct {
    uint8_t *nacp;
    uint8_t *icon;
    uint8_t *romfs;
    size_t offset;
    char name[512];
    char author[256];
    char version[16];
} Asset;

typedef struct {
    char *filename;
    uint8_t *data;
    size_t nrosize;
    Asset asset;
} Editor;

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
        fprintf(stderr, "Invalid NRO file format!\n");
        free(editor->data);
        return 0;
    }
    editor->nrosize = *(uint32_t *)(data + 0x18);
    if (filesize > editor->nrosize + 4 && memcmp(data + editor->nrosize, ASET_MAGIC, 4) == 0) {
        loadAsset(&editor->asset, data + editor->nrosize, filesize - editor->nrosize);
    }
    return 1;
}

void scanDirectoryForNROs(const char *dirpath, int depth, FILE *outputFile) {
    if (depth > 2) return;
    DIR *dir = opendir(dirpath);
    if (!dir) {
        printf("[ERROR] Failed to open directory: %s\n", dirpath);
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            const char *filename = entry->d_name;
            size_t len = strlen(filename);
            if (len > 4 && strcmp(filename + len - 4, ".nro") == 0) {
                char filepath[1024];
                snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, filename);
                Editor editor = {0};
                editor.filename = filepath;
                if (loadBinaryData(&editor)) {
                    Asset *asset = &editor.asset;
                    printf("%s\n", asset->name);
                    fprintf(outputFile, "%s%s\n", "Name: ", asset->name);
                    fprintf(outputFile, "%s%s\n", "Author: ", asset->author);
                    fprintf(outputFile, "%s%s\n", "Version: ", asset->version);
                    fprintf(outputFile, "%s%s\n\n", "Path: ", filepath);
                    free(asset->nacp);
                    free(asset->icon);
                    free(asset->romfs);
                    free(editor.data);
                }
                consoleUpdate(NULL);
                
            }
        } else if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char subdirpath[1024];
                snprintf(subdirpath, sizeof(subdirpath), "%s/%s", dirpath, entry->d_name);
                scanDirectoryForNROs(subdirpath, depth + 1, outputFile);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char **argv) {
    consoleInit(NULL);
    printf("Scanning /switch/ dirrectory for nro files\n");
    FILE *outputFile = fopen("/list.txt", "w");
    fprintf(outputFile, "%s\n\n", "Applications");
    const char *dirpath = "/switch"; 
    scanDirectoryForNROs(dirpath, 0, outputFile);
    fclose(outputFile); 
    printf("\nExpoerted to /list.txt");
    while (appletMainLoop()) {
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return 0;
}
