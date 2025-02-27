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
static int subSelected = 1;
static char ovlNames[150][100];
static char ovlAuthors[150][100];
static char ovlVersions[150][100];
static char fullOvlNames[150][100];
static char fullOvlAuthors[150][100];
static char fullOvlVersions[150][100];
static char ovlFileName[150][100];
static char ovlPath[150][100];
static int page = 1;
static int maxPages = 1;
static bool inDetaisMenu = false;
static bool inOptionsMenu = false;
static int exitFlag = 1;

static void clearVariables() {
    foundApps = 0;
    foundAppsPage = 0;
    selected = 1;
    selectedInPage = 1;
    subSelected = 1;
    for (int i = 0; i < 150; i++) {
        memset(ovlNames[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(ovlAuthors[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(ovlVersions[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(fullOvlNames[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(fullOvlAuthors[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(fullOvlVersions[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(ovlFileName[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(ovlPath[i], '\0', 50);
    }
    page = 1;
    maxPages = 1;
    inDetaisMenu = false;
    inOptionsMenu = false;
}
static void displayList() {
    printf(CONSOLE_ESC(6;1H));
    for (int i = (((page-1) * 35) + 1); i < ((page * 35) + 1); i++) {
        printf(CONSOLE_ESC(1C));
        printf("%-31s|%-25s|%-20s\n", ovlNames[i], ovlAuthors[i], ovlVersions[i]);
    }
}
static void clearSelected() {
    printf(CONSOLE_ESC(5;1H));
    for (int i = 0; i < selectedInPage; i++) {
        printf(CONSOLE_ESC(1B));
    }
    printf(CONSOLE_ESC(1C));
    printf("%-31s|%-25s|%-20s\n", ovlNames[selected], ovlAuthors[selected], ovlVersions[selected]);
}
static void displaySelected() {
    printf(CONSOLE_ESC(5;1H));
    for (int i = 0; i < selectedInPage; i++) {
        printf(CONSOLE_ESC(1B));
    }
    printf(CONSOLE_ESC(1C) CONSOLE_ESC(48;5;19m));
    printf("%-31s|%-25s|%-20s\n", ovlNames[selected], ovlAuthors[selected], ovlVersions[selected]);
    printf(CONSOLE_ESC(0m));
}
static void scanDirectoryForOVLs(const char *dirpath, int depth) {
    if (depth > 2) return;
    DIR *dir = opendir(dirpath);
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
                    foundApps = foundApps + 1;
                    foundAppsPage = foundAppsPage + 1;
                    if (foundAppsPage == 35) {
                        foundAppsPage = 0;
                        maxPages = maxPages + 1;
                    }
                    printf(CONSOLE_ESC(8;2H) "                                                                              ");
                    printf(CONSOLE_ESC(8;2H));
                    printf("%s%d%s%s", "Overlays [", foundApps, "]: ", asset->name);
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
                    strcpy(fullOvlNames[foundApps], asset->name);
                    strcpy(fullOvlAuthors[foundApps], asset->author);
                    strcpy(fullOvlVersions[foundApps], asset->version);
                    strcpy(ovlFileName[foundApps], filename);
                    strcpy(ovlPath[foundApps], dirpath);
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
                    strcpy(ovlNames[foundApps], asset->name);
                    strcpy(ovlAuthors[foundApps], asset->author);
                    strcpy(ovlVersions[foundApps], asset->version);
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
                scanDirectoryForOVLs(subdirpath, depth + 1);
            }
        }
    }
    closedir(dir);
}
int listOverlays(){
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    drawBox();
    printDetails();
    drawTop();
    drawBottom();
    printf(CONSOLE_ESC(3;35H) "List overlays");
    printf(CONSOLE_ESC(43;3H) "L/R - Change pages | Up/Down - Move | Y - Options | B - Back | A - Details");
    printf(CONSOLE_ESC(6;2H) "Scanning for installed overlays.");
    printf(CONSOLE_ESC(8;2H));
    printf("%s", "Overlays [0]: ");
    scanDirectoryForOVLs("/switch", 0);
    printf(CONSOLE_ESC(3;29H));
    printf("%s%d%s%d", "List overlays - Page ", page, "/", maxPages);
    drawFirstLine();
    drawLastLine();
    displayList();
    displaySelected();
    while (appletMainLoop()){
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        updateDetails();
        if (kDown & HidNpadButton_A) {
            if (!inDetaisMenu && !inOptionsMenu) {
                drawDetailsBox();
                char fullPath[1024];
                snprintf(fullPath, sizeof(fullPath), "%s/%s", ovlPath[selected], ovlFileName[selected]);
                double fileSize = getFileSize(fullPath);
                printf(CONSOLE_ESC(17;35H) "More details");
                printf(CONSOLE_ESC(19;21H));
                printf("%s%s","Name: ", fullOvlNames[selected]);
                printf(CONSOLE_ESC(20;21H));
                printf("%s%s","Author: ", fullOvlAuthors[selected]);
                printf(CONSOLE_ESC(21;21H));
                printf("%s%s","Version: ", fullOvlVersions[selected]);
                printf(CONSOLE_ESC(22;21H));
                printf("%s%.2f MB","File size: ", fileSize);
                printf(CONSOLE_ESC(23;21H));
                printf("%s%s","File name: ", ovlFileName[selected]);
                printf(CONSOLE_ESC(24;21H));
                printf("%s%s","Path: ", ovlPath[selected]);
                inDetaisMenu = true;
            }
            if (inOptionsMenu) {
                char fullPath[1024];
                snprintf(fullPath, sizeof(fullPath), "%s/%s", ovlPath[selected], ovlFileName[selected]);
                remove(fullPath);
                clearVariables();
                exitFlag = 2;
                break;
            }
        }
        if (kDown & HidNpadButton_AnyUp) {
            if (!inDetaisMenu && !inOptionsMenu) {
                u64 startTime = armGetSystemTick();
                bool held = false;
                while (padGetButtons(&pad) & HidNpadButton_AnyUp) {
                    if (armGetSystemTick() - startTime > armGetSystemTickFreq() / 3) {
                        held = true;
                        break;
                    }
                    padUpdate(&pad);
                }
                if (held) {
                    while (padGetButtons(&pad) & HidNpadButton_AnyUp) {
                        if (selectedInPage != 1) {
                            clearSelected();
                            selected = selected - 1;
                            selectedInPage = selectedInPage - 1;
                            displaySelected();
                        }
                        consoleUpdate(NULL);
                        svcSleepThread(40000000);
                        padUpdate(&pad);
                    }
                } else {
                    if (selectedInPage != 1) {
                        clearSelected();
                        selected = selected - 1;
                        selectedInPage = selectedInPage - 1;
                        displaySelected();
                    }
                }
            }
        }
        if (kDown & HidNpadButton_AnyDown) {
            if (!inDetaisMenu && !inOptionsMenu) {
                u64 startTime = armGetSystemTick();
                bool held = false;
                while (padGetButtons(&pad) & HidNpadButton_AnyDown) {
                    if (armGetSystemTick() - startTime > armGetSystemTickFreq() / 3) {
                        held = true;
                        break;
                    }
                    padUpdate(&pad);
                }
                if (held) {
                    while (padGetButtons(&pad) & HidNpadButton_AnyDown) {
                        if (selectedInPage != 35 && selected != foundApps) {
                            clearSelected();
                            selected = selected + 1;
                            selectedInPage = selectedInPage + 1;
                            displaySelected();
                        }
                        consoleUpdate(NULL);
                        svcSleepThread(40000000);
                        padUpdate(&pad);
                    }
                } else {
                    if (selectedInPage != 35 && selected != foundApps) {
                        clearSelected();
                        selected = selected + 1;
                        selectedInPage = selectedInPage + 1;
                        displaySelected();
                    }
                }
            }
        }
        if (kDown & HidNpadButton_L) {
            if (!inDetaisMenu && !inOptionsMenu) {
                if (page != 1) {
                    page = page - 1;
                    selected = (((page-1) * 35) + 1);
                    selectedInPage = 1;
                    printf(CONSOLE_ESC(3;29H));
                    printf("%s%d%s%d", "List overlays - Page ", page, "/", maxPages);
                    displayList();
                    displaySelected();
                }
            }
        }
        if (kDown & HidNpadButton_R) {
            if (!inDetaisMenu && !inOptionsMenu) {
                if (page != maxPages) {
                    page = page + 1;
                    selected = (((page-1) * 35) + 1);
                    selectedInPage = 1;
                    printf(CONSOLE_ESC(3;29H));
                    printf("%s%d%s%d", "List overlays - Page ", page, "/", maxPages);
                    displayList();
                    displaySelected();
                }
            }
        }
        if (kDown & HidNpadButton_B) {
            if (inDetaisMenu || inOptionsMenu) {
                displayList();
                displaySelected();
                subSelected = 1;
                inDetaisMenu = false;
                inOptionsMenu = false;
            } else {
                clearVariables();
                exitFlag = 1;
                break;
            }
        }
        if (kDown &  HidNpadButton_Y) {
            if (!inOptionsMenu && !inDetaisMenu) {
                drawOptionsBox();
                printf(CONSOLE_ESC(19;38H) "Options");
                printf(CONSOLE_ESC(21;31H) CONSOLE_ESC(48;5;19m) "Delete overlay      " CONSOLE_ESC(0m));
                inOptionsMenu = true;
            }
        }
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    if (exitFlag == 1) {
        return 1;
    } else if (exitFlag == 2) {
        listOverlays();
    }
    return 0;
}