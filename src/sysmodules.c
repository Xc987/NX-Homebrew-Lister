#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "main.h"
#include "boxgui.h"
#include "read.h"

static int foundSysmodules = 0;
static int foundSysmodulesPage = 0;
static int selected = 1;
static int selectedInPage = 1;
static int subSelected = 1;
static char sysNames[150][100];
static char sysTID[150][100];
static char sysStatus[150][100];
static char fullSysNames[150][100];
static char sysFlag[150][10];
static char sysPath[150][100];
static int page = 1;
static int maxPages = 1;
static bool inDetaisMenu = false;
static bool inOptionsMenu = false;
static int exitFlag = 1;

static void clearVariables() {
    foundSysmodules = 0;
    foundSysmodulesPage = 0;
    selected = 1;
    selectedInPage = 1;
    subSelected = 1;
    for (int i = 0; i < 150; i++) {
        memset(sysNames[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(sysTID[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(sysStatus[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(fullSysNames[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(sysFlag[i], '\0', 50);
    }
    for (int i = 0; i < 150; i++) {
        memset(sysPath[i], '\0', 50);
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
        printf("%-31s|%-25s|%-20s\n", sysNames[i], sysTID[i], sysStatus[i]);
    }
}
static void clearSelected() {
    printf(CONSOLE_ESC(5;1H));
    for (int i = 0; i < selectedInPage; i++) {
        printf(CONSOLE_ESC(1B));
    }
    printf(CONSOLE_ESC(1C));
    printf("%-31s|%-25s|%-20s\n", sysNames[selected], sysTID[selected], sysStatus[selected]);
}
static void displaySelected() {
    printf(CONSOLE_ESC(5;1H));
    for (int i = 0; i < selectedInPage; i++) {
        printf(CONSOLE_ESC(1B));
    }
    printf(CONSOLE_ESC(1C) CONSOLE_ESC(48;5;19m));
    printf("%-31s|%-25s|%-20s\n", sysNames[selected], sysTID[selected], sysStatus[selected]);
    printf(CONSOLE_ESC(0m));
}
static void clearSubSelected() {
    printf(CONSOLE_ESC(21;31H) CONSOLE_ESC(48;5;236m) "Delete sysmodule    " CONSOLE_ESC(0m));
    if (strcmp(sysStatus[selected], "Disabled") == 0 ) {
        printf(CONSOLE_ESC(22;31H) CONSOLE_ESC(48;5;236m) "Enable sysmodule    " CONSOLE_ESC(0m));
    } else {
        printf(CONSOLE_ESC(22;31H) CONSOLE_ESC(48;5;236m) "Disable sysmodule   " CONSOLE_ESC(0m));
    }
}
static void displaySubSelected() {
    if (subSelected == 1) {
        printf(CONSOLE_ESC(21;31H) CONSOLE_ESC(48;5;19m) "Delete sysmodule    " CONSOLE_ESC(0m));
    } else if (subSelected == 2) {
        if (strcmp(sysStatus[selected], "Disabled") == 0 ) {
            printf(CONSOLE_ESC(22;31H) CONSOLE_ESC(48;5;19m) "Enable sysmodule    " CONSOLE_ESC(0m));
        } else {
            printf(CONSOLE_ESC(22;31H) CONSOLE_ESC(48;5;19m) "Disable sysmodule   " CONSOLE_ESC(0m));
        }
    }
}
static void scanDirectoryForSYS(const char* basePath) {
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
                    char* requiresRebootValue = extractValueForKey(jsonContent, "requires_reboot");
                    foundSysmodules = foundSysmodules + 1;
                    foundSysmodulesPage = foundSysmodulesPage + 1;
                    if (foundSysmodulesPage == 35) {
                        foundSysmodulesPage = 0;
                        maxPages = maxPages + 1;
                    }
                    printf(CONSOLE_ESC(8;2H) "                                                                              ");
                    printf(CONSOLE_ESC(8;2H));
                    printf("%s%d%s%s", "Sysmodules [", foundSysmodules, "]: ", nameValue ? nameValue : "N/A");
                    int length = strlen(nameValue ? nameValue : "N/A");
                    if (length > 34) {
                        nameValue[33] = '\0';
                        strcat(nameValue ? nameValue : "N/A", ".");
                    }
                    strcpy(fullSysNames[foundSysmodules], nameValue ? nameValue : "N/A");
                    strcpy(sysPath[foundSysmodules], "/atmosphere/contents/");
                    if (length > 31) {
                        nameValue[30] = '\0';
                        strcat(nameValue ? nameValue : "N/A", ".");
                    }
                    strcpy(sysNames[foundSysmodules], nameValue ? nameValue : "N/A");
                    strcpy(sysTID[foundSysmodules], tidValue ? tidValue : "N/A");
                    strcpy(sysFlag[foundSysmodules], requiresRebootValue ? requiresRebootValue : "N/A");
                    length = strlen(nameValue ? nameValue : "N/A");
                    

                    if (fileExists(boot2FlagPath)) {
                        strcpy(sysStatus[foundSysmodules], "Enabled");
                    } else {
                        strcpy(sysStatus[foundSysmodules], "Disabled");
                    }
                    free(nameValue);
                    free(tidValue);
                    free(requiresRebootValue);
                }
                updateDetails();
                consoleUpdate(NULL);
            }
        }
    }
    closedir(dir);
}
int listSysmodules(){
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    drawBox();
    printDetails();
    drawTop();
    drawBottom();
    printf(CONSOLE_ESC(3;34H) "List sysmodules");
    printf(CONSOLE_ESC(43;3H) "L/R - Change pages | Up/Down - Move | Y - Options | B - Back | A - Details");
    printf(CONSOLE_ESC(6;2H) "Scanning for installed sysmodules.");
    printf(CONSOLE_ESC(8;2H));
    printf("%s", "sysmodules [0]: ");
    scanDirectoryForSYS("/atmosphere/contents/");
    printf(CONSOLE_ESC(3;28H));
    printf("%s%d%s%d", "List sysmodules - Page ", page, "/", maxPages);
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
                snprintf(fullPath, sizeof(fullPath), "%s/%s", sysPath[selected], sysTID[selected]);
                long totalSize = getDirectorySize(fullPath);
                printf(CONSOLE_ESC(17;35H) "More details");
                printf(CONSOLE_ESC(19;21H));
                printf("%s%s","Name: ", fullSysNames[selected]);
                printf(CONSOLE_ESC(20;21H));
                printf("%s%s","TID: ", sysTID[selected]);
                printf(CONSOLE_ESC(21;21H));
                printf("%s%s","Status: ", sysStatus[selected]);
                printf(CONSOLE_ESC(22;21H));
                printf("%s%s","Requires reboot: ", sysFlag[selected]);
                printf(CONSOLE_ESC(23;21H));
                printf("Dir size: %.2f MB", totalSize / (1024.0 * 1024.0));
                printf(CONSOLE_ESC(24;21H));
                printf("%s%s","Path: ", sysPath[selected]);
                inDetaisMenu = true;
            }
            if (inOptionsMenu) {
                if (subSelected == 1) {
                    char fullPath[1024];
                    snprintf(fullPath, sizeof(fullPath), "%s%s", sysPath[selected], sysTID[selected]);
                    fsdevDeleteDirectoryRecursively(fullPath);
                    clearVariables();
                    exitFlag = 2;
                    break;
                } else if (subSelected == 2) {
                    if (strcmp(sysStatus[selected], "Disabled") == 0 ) {
                        char fullPath[1024];
                        snprintf(fullPath, sizeof(fullPath), "%s%s/flags/boot2.flag", sysPath[selected], sysTID[selected]);
                        fopen(fullPath, "wb");
                        strcpy(sysStatus[selected], "Enabled");
                    } else {
                        char fullPath[1024];
                        snprintf(fullPath, sizeof(fullPath), "%s%s/flags/boot2.flag", sysPath[selected], sysTID[selected]);
                        remove(fullPath);
                        strcpy(sysStatus[selected], "Disabled");
                    }
                    clearSubSelected();
                    displaySubSelected();
                }
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
            if (inOptionsMenu) {
                if (subSelected != 1) {
                    clearSubSelected();
                    subSelected = subSelected - 1;
                    displaySubSelected();
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
                        if (selectedInPage != 35 && selected != foundSysmodules) {
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
                    if (selectedInPage != 35 && selected != foundSysmodules) {
                        clearSelected();
                        selected = selected + 1;
                        selectedInPage = selectedInPage + 1;
                        displaySelected();
                    }
                }
            }
            if (inOptionsMenu) {
                if (subSelected != 2) {
                    clearSubSelected();
                    subSelected = subSelected + 1;
                    displaySubSelected();
                }
            }
        }
        if (kDown & HidNpadButton_L) {
            if (!inDetaisMenu && !inOptionsMenu) {
                if (page != 1) {
                    page = page - 1;
                    selected = (((page-1) * 35) + 1);
                    selectedInPage = 1;
                    printf(CONSOLE_ESC(3;28H));
                    printf("%s%d%s%d", "List sysmodules - Page ", page, "/", maxPages);
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
                    printf(CONSOLE_ESC(3;28H));
                    printf("%s%d%s%d", "List sysmodules - Page ", page, "/", maxPages);
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
                printf(CONSOLE_ESC(21;31H) CONSOLE_ESC(48;5;19m) "Delete sysmodule    " CONSOLE_ESC(0m));
                if (strcmp(sysStatus[selected], "Disabled") == 0 ) {
                    printf(CONSOLE_ESC(22;31H) CONSOLE_ESC(48;5;236m) "Enable sysmodule" CONSOLE_ESC(0m));
                } else {
                    printf(CONSOLE_ESC(22;31H) CONSOLE_ESC(48;5;236m) "Disable sysmodule" CONSOLE_ESC(0m));
                }
                inOptionsMenu = true;
            }
        }
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    if (exitFlag == 1) {
        return 1;
    } else if (exitFlag == 2) {
        listSysmodules();
    }
    return 0;
}