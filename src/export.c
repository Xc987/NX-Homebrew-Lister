#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "main.h"

#define NRO_MAGIC "NRO0"
#define ASET_MAGIC "ASET"
#define TOOLBOX_FILE "toolbox.json"
#define FLAGS_DIR "flags"
#define BOOT2_FLAG "boot2.flag"
#define PREFIX "0100"

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

void scanDirectoryForSYS(const char* basePath, FILE *outputFile) {
    DIR* dir = opendir(basePath);
    if (!dir) {
        return;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char subfolderPath[PATH_MAX];
            snprintf(subfolderPath, PATH_MAX, "%s%s/", basePath, entry->d_name);
            char toolboxPath[PATH_MAX];
            int ret = snprintf(toolboxPath, PATH_MAX, "%s%s", subfolderPath, TOOLBOX_FILE);
            if (ret < 0) {
                abort();
            }
            FILE* testFile = fopen(toolboxPath, "rb");
            if (testFile) {
                fclose(testFile);
                char flagsPath[PATH_MAX];
                int ret = snprintf(flagsPath, PATH_MAX, "%s%s/", subfolderPath, FLAGS_DIR);
                if (ret < 0) {
                    abort();
                }
                char boot2FlagPath[PATH_MAX];
                ret = snprintf(boot2FlagPath, PATH_MAX, "%s%s", flagsPath, BOOT2_FLAG);
                if (ret < 0) {
                    abort();
                }
                FILE* jsonFile = fopen(toolboxPath, "r");
                if (jsonFile) {
                    char jsonContent[2048] = {0};
                    fread(jsonContent, 1, sizeof(jsonContent) - 1, jsonFile);
                    fclose(jsonFile);
                    removeCR(jsonContent);
                    char* nameValue = extractValueForKey(jsonContent, "name");
                    char* tidValue = extractValueForKey(jsonContent, "tid");
                    //char* requiresRebootValue = extractValueForKey(jsonContent, "requires_reboot");
                    printf("%s\n", nameValue ? nameValue : "N/A");
                    fprintf(outputFile, "%s%s\n", "Name: ", nameValue ? nameValue : "N/A");
                    fprintf(outputFile, "%s%s\n", "TID: ", tidValue ? tidValue : "N/A");
                    if (fileExists(boot2FlagPath)) {
                        fprintf(outputFile, "%s%s\n\n", "Enabled: ", "Yes");
                    } else {
                        fprintf(outputFile, "%s%s\n\n", "Enabled: ", "No");
                    }                    
                    free(nameValue);
                    free(tidValue);
                }
                consoleUpdate(NULL);
            }
        }
    }
    closedir(dir);
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
void scanDirectoryForOVLs(const char *dirpath, int depth, FILE *outputFile) {
    if (depth > 1) return;
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
            if (len > 4 && strcmp(filename + len - 4, ".ovl") == 0) {
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
                scanDirectoryForOVLs(subdirpath, depth + 1, outputFile);
            }
        }
    }
    closedir(dir);
}

void scanForPayloads(FILE *outputFile) {
    DIR *dir;
    struct dirent *ent;
    const char *path = "/bootloader/payloads/";
    dir = opendir(path);
    if (dir == NULL) {
        return;
    }
    while ((ent = readdir(dir)) != NULL) {
        printf("%s\n", ent->d_name);
        fprintf(outputFile, "%s%s\n", "File: ", ent->d_name);
        consoleUpdate(NULL);
    }
    closedir(dir);
}
void scanForPatches(FILE *outputFile) {
    DIR* dir = opendir("/atmosphere/exefs_patches/");
    if (dir == NULL) {
        return;
    } else {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
                printf("%s\n", entry->d_name);
                fprintf(outputFile, "%s%s\n", "Folder: ", entry->d_name);
                consoleUpdate(NULL);
            }
        }
        closedir(dir);
    }
}
void contains_special_files(const char *folder_path, FILE *outputFile) {
    const char *subfolders[] = {"cheats", "romfs", "exefs"};
    const char *file_name = "icon.jpg";
    struct stat statbuf;

    // Check for each subfolder
    for (int i = 0; i < 3; i++) {
        char subfolder_path[256];
        snprintf(subfolder_path, sizeof(subfolder_path), "%s/%s", folder_path, subfolders[i]);

        if (stat(subfolder_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            fprintf(outputFile, "%s%s\n", "Content: ", subfolders[i]);
        }
    }

    // Check for the icon.jpg file
    char icon_path[256];
    snprintf(icon_path, sizeof(icon_path), "%s/%s", folder_path, file_name);

    if (stat(icon_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
        fprintf(outputFile, "%s%s\n", "Content: ", file_name);
    }
}

void scanForContent(const char *prefix, FILE *outputFile) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir("/atmosphere/contents/");
    if (!dir) {
        return;
    }

    printf("Scanning %s...\n\n", "/atmosphere/contents/");

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strncmp(entry->d_name, prefix, strlen(prefix)) == 0) {
            if (entry->d_name[4] == '0') {
                continue;
            }
            char modified_name[34];
            snprintf(modified_name, sizeof(modified_name), "0x%.30s", entry->d_name);
            Result rc=0;
            u64 application_id = strtoull(modified_name, NULL, 16);
            NsApplicationControlData *buf=NULL;
            u64 outsize=0;

            NacpLanguageEntry *langentry = NULL;
            char name[0x201];

            buf = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
            if (buf==NULL) {
                rc = MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);
                printf("Failed to alloc mem.\n");
            }
            else {
                memset(buf, 0, sizeof(NsApplicationControlData));
            }
            if (R_SUCCEEDED(rc)) {
                rc = nsInitialize();
                if (R_FAILED(rc)) {
                    printf("nsInitialize() failed: 0x%x\n", rc);
                }
            }
            if (R_SUCCEEDED(rc)) {
                rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, application_id, buf, sizeof(NsApplicationControlData), &outsize);
                if (R_FAILED(rc)) {
                    printf("nsGetApplicationControlData() failed: 0x%x\n", rc);
                }

                if (outsize < sizeof(buf->nacp)) {
                    rc = -1;
                    printf("Outsize is too small: 0x%lx.\n", outsize);
                }

                if (R_SUCCEEDED(rc)) {
                    rc = nacpGetLanguageEntry(&buf->nacp, &langentry);
                    if (R_FAILED(rc) || langentry==NULL) printf("Failed to load LanguageEntry.\n");
                }

                if (R_SUCCEEDED(rc)) {
                    memset(name, 0, sizeof(name));
                    strncpy(name, langentry->name, sizeof(name)-1);//Don't assume the nacp string is NUL-terminated for safety.
                    printf("%s\n", name);
                    fprintf(outputFile, "%s%s\n", "Name: ", name);
                    fprintf(outputFile, "%s%s\n", "TID: ", entry->d_name);
                    char full_folder_path[300];
                    snprintf(full_folder_path, sizeof(full_folder_path), "%s/%s", "/atmosphere/contents/" , entry->d_name);
                    contains_special_files(full_folder_path, outputFile);
                    fprintf(outputFile, "\n");
                    
                }
                nsExit();
            }
            free(buf);
            consoleUpdate(NULL);
        }
    }

    closedir(dir);
}
int exportall() {
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    const char *dirpath = "/switch"; 
    printf("Scanning for installed homebrew software\n");
    FILE *outputFile = fopen("/list.txt", "w");

    fprintf(outputFile, "%s\n\n\n", "Applications");
    printf("\n%s\n\n", "Applications");
    scanDirectoryForNROs(dirpath, 0, outputFile);

    fprintf(outputFile, "\n%s\n\n\n", "Overlays");
    printf("\n%s\n\n", "Overlays");
    scanDirectoryForOVLs(dirpath, 0, outputFile);

    fprintf(outputFile, "\n%s\n\n\n", "Sysmodules");
    printf("\n%s\n\n", "Sysmodules");
    dirpath = "/atmosphere/contents/";
    scanDirectoryForSYS(dirpath, outputFile);

    fprintf(outputFile, "\n%s\n\n\n", "Payloads");
    printf("\n%s\n\n", "Payloads");
    scanForPayloads(outputFile);

    fprintf(outputFile, "\n\n%s\n\n\n", "exeFS Patches");
    printf("\n%s\n\n", "exeFS Patches");
    scanForPatches(outputFile);

    fprintf(outputFile, "\n\n%s\n\n\n", "External game content");
    printf("\n%s\n\n", "External game content");
    scanForContent(PREFIX, outputFile);

    fclose(outputFile); 
    printf("\nExpoerted to /list.txt");
    while (appletMainLoop()) {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        if (kDown & HidNpadButton_Plus) {
            break;
        }
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return 0;
}