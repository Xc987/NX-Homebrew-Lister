// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // For directory operations
#include <switch.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "nro.hpp"

#define MAX_ENTRIES 1024
#define DELAY_FRAMES 5

// Function to check if a string ends with .nro
bool endsWithNRO(const char* filename) {
    size_t len = strlen(filename);
    return len > 4 && strcmp(filename + len - 4, ".nro") == 0;
}

// Function to scan a directory for .nro files
void scanForNROs(const char* directoryPath) {
    DIR* dir = opendir(directoryPath);
    if (dir == NULL) {
        printf("Failed to open directory: %s\n", directoryPath);
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char filePath[512];
        snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);

        struct stat pathStat;
        stat(filePath, &pathStat);

        if (S_ISDIR(pathStat.st_mode)) {
            // Recursively scan subdirectories
            scanForNROs(filePath);
        } else if (endsWithNRO(entry->d_name)) {
            // Wait for user input to display the .nro file
            while (appletMainLoop()) {
                PadState pad;
                padInitializeDefault(&pad);
                padUpdate(&pad);

                u64 kHeld = padGetButtons(&pad);
                if (kHeld & HidNpadButton_A) {
                    const char* version = getmeta(filePath);
                    const char* name = getmetaname(filePath);
                    printf("%s - %s - %s\n", name, entry->d_name, version);
                    break;
                }

                u64 kDown = padGetButtonsDown(&pad);
                if (kDown & HidNpadButton_Plus) {
                    consoleExit(NULL);
                    exit(0);
                }

                consoleUpdate(NULL);
            }
        }
    }

    closedir(dir);
}

// Main program entrypoint
int main(int argc, char* argv[]) {
    // Initialize the console for text output
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Print a message to indicate the program has started
    printf("Scanning for .nro files in /switch/...\n\n");

    // Directory path to start scanning
    const char* basePath = "sdmc:/switch/";

    // Check if the base directory exists
    DIR* baseDir = opendir(basePath);
    if (baseDir == NULL) {
        printf("Failed to open base directory: %s\n", basePath);
        consoleExit(NULL);
        return 1;
    }
    closedir(baseDir);

    // Start scanning for .nro files
    scanForNROs(basePath);

    // Wait for user input to exit
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

    // Cleanup and exit
    consoleExit(NULL);
    return 0;
}
