#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "main.h"
#include "boxgui.h"
#include "read.h"

static int foundApps = 0;
static int foundAppsPage = 0;
static int selected = 1;
static int selectedInPage = 1;
char appNames[150][100];
char appAuthors[150][100];
char appVersions[150][100];
char fullAppNames[150][100];
char fullAppAuthors[150][100];
char fullAppVersions[150][100];
char appStars[150][10];
char appFileName[150][100];
char appPath[150][100];
int page = 1;
int maxPages = 1;
bool inDetaisMenu = false;
bool inOptionsMenu = false;


void displayList() {
    printf(CONSOLE_ESC(6;1H));
    for (int i = (((page-1) * 35) + 1); i < ((page * 35) + 1); i++) {
        printf(CONSOLE_ESC(1C));
        printf("%-31s|%-25s|%-20s\n", appNames[i], appAuthors[i], appVersions[i]);
    }
}
void clearSelected() {
    printf(CONSOLE_ESC(5;1H));
    for (int i = 0; i < selectedInPage; i++) {
        printf(CONSOLE_ESC(1B));
    }
    printf(CONSOLE_ESC(1C));
    printf("%-31s|%-25s|%-20s\n", appNames[selected], appAuthors[selected], appVersions[selected]);
}
void displaySelected() {
    printf(CONSOLE_ESC(5;1H));
    for (int i = 0; i < selectedInPage; i++) {
        printf(CONSOLE_ESC(1B));
    }
    printf(CONSOLE_ESC(1C) CONSOLE_ESC(48;5;19m));
    printf("%-31s|%-25s|%-20s\n", appNames[selected], appAuthors[selected], appVersions[selected]);
    printf(CONSOLE_ESC(0m));
}

static void scanDirectoryForNROs(const char *dirpath, int depth) {
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
                    int hasStar = checkStarFile(dirpath, filename);
                    foundApps = foundApps + 1;
                    foundAppsPage = foundAppsPage + 1;
                    if (foundAppsPage == 35) {
                        foundAppsPage = 0;
                        maxPages = maxPages + 1;
                    }
                    printf(CONSOLE_ESC(8;2H) "                                                                              ");
                    printf(CONSOLE_ESC(8;2H));
                    printf("%s%d%s%s", "Applications [", foundApps, "]: ", asset->name);
                    int length = strlen(asset->name);
                    if (length > 34) {
                        asset->name[33] = '\0';
                        strcat(asset->name, ".");
                    }
                    length = strlen(asset->author);
                    if (length > 32) {
                        asset->author[31] = '\0';
                        strcat(asset->author, ".");
                    }
                    strcpy(fullAppNames[foundApps], asset->name);
                    strcpy(fullAppAuthors[foundApps], asset->author);
                    strcpy(fullAppVersions[foundApps], asset->version);
                    strcpy(appFileName[foundApps], filename);
                    strcpy(appPath[foundApps], dirpath);
                    length = strlen(asset->name);
                    if (length > 31) {
                        asset->name[30] = '\0';
                        strcat(asset->name, ".");
                    }
                    length = strlen(asset->author);
                    if (length > 25) {
                        asset->author[24] = '\0';
                        strcat(asset->author, ".");
                    }
                    length = strlen(asset->version);
                    if (length > 20) {
                        asset->version[19] = '\0';
                        strcat(asset->version, ".");
                    }
                    strcpy(appNames[foundApps], asset->name);
                    strcpy(appAuthors[foundApps], asset->author);
                    strcpy(appVersions[foundApps], asset->version);
                    if (hasStar == 1) {
                        strcpy(appStars[foundApps], "Yes");
                    } else {
                        strcpy(appStars[foundApps], "No");
                    }
                    free(asset->nacp);
                    free(asset->icon);
                    free(asset->romfs);
                    free(editor.data);
                }
                updateDetails();
                consoleUpdate(NULL);
            }
        } else if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char subdirpath[1024];
                snprintf(subdirpath, sizeof(subdirpath), "%s/%s", dirpath, entry->d_name);
                scanDirectoryForNROs(subdirpath, depth + 1);
            }
        }
    }
    closedir(dir);
}
void print_file_size_in_mb(const char *file_path) {
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        return;
    }
    double size_in_mb = (double)file_stat.st_size / (1024 * 1024);
    printf(CONSOLE_ESC(23;32H));
    printf("%.2f MB\n", size_in_mb);
}
void printDirectorySize(const char* path, long* totalSize) {
    DIR* dir;
    struct dirent* entry;
    struct stat fileStat;
    dir = opendir(path);
    if (!dir) {
        printf("Failed to open directory: %s\n", path);
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char fullPath[PATH_MAX];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
        
        if (stat(fullPath, &fileStat) == 0) {
            if (S_ISDIR(fileStat.st_mode)) {
                printDirectorySize(fullPath, totalSize);
            } else {
                *totalSize += fileStat.st_size;
            }
        }
    }
    closedir(dir);
}
int listApps(){
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    drawBox();
    printDetails();
    drawTop();
    drawBottom();
    printf(CONSOLE_ESC(3;33H) "List applications");
    printf(CONSOLE_ESC(43;3H) "L/R - Change pages | Up/Down - Scroll | Y - Options | B - Back | A - Details");
    printf(CONSOLE_ESC(6;2H) "Scanning for installed applications.");
    printf(CONSOLE_ESC(8;2H));
    printf("%s", "Applications [0]: ");
    scanDirectoryForNROs("/switch", 0);
    printf(CONSOLE_ESC(3;27H));
    printf("%s%d%s%d", "List applications - Page ", page, "/", maxPages);
    drawFirstLine();
    drawLastLine();
    displayList();
    displaySelected();
    while (appletMainLoop()){
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        updateDetails();
        if (kDown & HidNpadButton_Plus) {
            break;
        }
        if (kDown & HidNpadButton_A) {
            if (!inDetaisMenu) {
                drawDetailsBox();
                printf(CONSOLE_ESC(17;35H) "More details");
                char* file_path;
                file_path = malloc(200);
                strcpy(file_path, appPath[selected]);
                strcat(file_path, "/");
                strcat(file_path, appFileName[selected]);
                print_file_size_in_mb(file_path);
                long totalSize = 0;
                printf(CONSOLE_ESC(24;31H));
                if (strcmp(appPath[selected], "/switch") == 0 ) {
                    printf("-");
                } else {
                    printDirectorySize(appPath[selected], &totalSize);
                    printf("%.2f MB\n", totalSize / (1024.0 * 1024.0));
                }  
                printf(CONSOLE_ESC(19;21H));
                printf("%s%s","Name: ", fullAppNames[selected]);
                printf(CONSOLE_ESC(20;21H));
                printf("%s%s","Author: ", fullAppAuthors[selected]);
                printf(CONSOLE_ESC(21;21H));
                printf("%s%s","Version: ", fullAppVersions[selected]);
                printf(CONSOLE_ESC(22;21H));
                printf("%s%s","Starred: ", appStars[selected]);
                printf(CONSOLE_ESC(23;21H));
                printf("%s","File size: ");
                printf(CONSOLE_ESC(24;21H));
                printf("%s","Dir size: ");
                printf(CONSOLE_ESC(25;21H));
                printf("%s%s","File name: ", appFileName[selected]);
                printf(CONSOLE_ESC(26;21H));
                printf("%s%s","Path: ", appPath[selected]);
                inDetaisMenu = true;
            }
            
        }
        if (kDown & HidNpadButton_AnyUp) {
            if (!inDetaisMenu) {
                if (selectedInPage != 1) {
                    clearSelected();
                    selected = selected - 1;
                    selectedInPage = selectedInPage - 1;
                    displaySelected();
                }
            }
        }
        if (kDown & HidNpadButton_AnyDown) {
            if (!inDetaisMenu) {
                if (selectedInPage != 35 && selected != foundApps) {
                    clearSelected();
                    selected = selected + 1;
                    selectedInPage = selectedInPage + 1;
                    displaySelected();
                }
            }
        }
        if (kDown & HidNpadButton_L) {
            if (!inDetaisMenu) {
                if (page != 1) {
                    page = page - 1;
                    selected = (((page-1) * 35) + 1);
                    selectedInPage = 1;
                    printf(CONSOLE_ESC(3;27H));
                    printf("%s%d%s%d", "List applications - Page ", page, "/", maxPages);
                    displayList();
                    displaySelected();
                }
            }
        }
        if (kDown & HidNpadButton_R) {
            if (!inDetaisMenu) {
                if (page != maxPages) {
                    page = page + 1;
                    selected = (((page-1) * 35) + 1);
                    selectedInPage = 1;
                    printf(CONSOLE_ESC(3;27H));
                    printf("%s%d%s%d", "List applications - Page ", page, "/", maxPages);
                    displayList();
                    displaySelected();
                }
            }
        }
        if (kDown & HidNpadButton_B) {
            if (inDetaisMenu) {
                displayList();
                displaySelected();
                inDetaisMenu = false;
            }
        }
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return 0;
}