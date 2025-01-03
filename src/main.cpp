#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <switch.h>
#include "nro.hpp"


int number = 1;
char number_with_hash[10]; 
bool scrollNow = false;

bool endsWithNRO(const char* filename) {
    size_t len = strlen(filename);
    return len > 4 && strcmp(filename + len - 4, ".nro") == 0;
}

void scanForNROs(const char* directoryPath, int depth, bool* foundNRO) {
    if (depth > 1) {
        return;
    }
    DIR* dir = opendir(directoryPath);
    if (dir == NULL) {
        printf("Failed to open directory: %s\n", directoryPath);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char filePath[512];
        snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);
        struct stat pathStat;
        stat(filePath, &pathStat);

        if (S_ISDIR(pathStat.st_mode)) {
            if (depth < 2) {
                scanForNROs(filePath, depth + 1, foundNRO);
            }
        } else if (endsWithNRO(entry->d_name)) {
            *foundNRO = true;
            const char* version = getmetaversion(filePath);
            const char* name = getmetaname(filePath);
            const char* author = getmetaauthor(filePath);
            char str[100];
            strcpy(str, name);
            int length = strlen(str);
            if (length > 25) {
                str[24] = '\0'; 
                strcat(str, ".");
            }
            char str2[100];
            strcpy(str2, entry->d_name);
            length = strlen(str2);
            if (length > 4) {
                str2[length - 4] = '\0';
            } else {
                str2[0] = '\0';
            }
            length = strlen(str2);
            if (length > 20) {
                str2[19] = '\0';
                strcat(str2, ".");
            }
            char str3[100];
            strcpy(str3, author);
            length = strlen(str3);
            if (length > 20) {
                str3[19] = '\0';
                strcat(str3, ".");
            }
            char str4[100];
            strcpy(str4, version);
            length = strlen(str4);
            if (length > 7) {
                str4[6] = '\0';
                strcat(str4, ".");
            }
            PadState pad;
            padInitializeDefault(&pad);
            while (appletMainLoop()) {
                padUpdate(&pad);
                u64 kHeld = padGetButtons(&pad);
                if (kHeld & HidNpadButton_A || scrollNow) {
                    sprintf(number_with_hash, "#%d", number);
                    printf("%-3s|%-25s|%-20s|%-20s|%-7s\n", number_with_hash, str, str2, str3, str4);
                    number++;
                    consoleUpdate(NULL);
                    break;
                }
                u64 kDown = padGetButtonsDown(&pad);
                if (kDown & HidNpadButton_Plus) {
                    consoleExit(NULL);
                    exit(0);
                }
                if (kDown & HidNpadButton_Y ) {
                    scrollNow = true;
                }
                consoleUpdate(NULL);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    printf("Hold A to list installed apps or press Y to list automatically.\n");
    printf("--------------------------------------------------------------------------------");
    printf("%-3s|%-25s|%-20s|%-20s|%-7s\n", "#", "App name", "File name (.nro)", "Author", "Version");
    printf("%-3s|%-25s|%-20s|%-20s|%-7s", "---", "-------------------------", "--------------------", "--------------------", "--------");
    const char* basePath = "sdmc:/switch/";
    DIR* baseDir = opendir(basePath);
    if (baseDir == NULL) {
        printf("Failed to open base directory: %s\n", basePath);
        consoleExit(NULL);
        return 1;
    }
    closedir(baseDir);
    bool foundNRO = false;
    scanForNROs(basePath, 0, &foundNRO);

    if (foundNRO) {
        printf("--------------------------------------------------------------------------------");
        printf("All files and subfolders have been scanned. Press PLUS to exit.\n");
        consoleUpdate(NULL);
    }
    while (appletMainLoop()) {
        PadState pad;
        padInitializeDefault(&pad);
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
