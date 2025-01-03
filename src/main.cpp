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

// Function to scan a directory for .nro files with a depth limit
void scanForNROs(const char* directoryPath, int depth, bool* foundNRO) {
    if (depth > 2) {
        return; // Do not scan beyond 2 levels
    }

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
            if (depth < 2) {
                // Recursively scan subdirectories if within depth limit
                scanForNROs(filePath, depth + 1, foundNRO);
            }
        } else if (endsWithNRO(entry->d_name)) {
            *foundNRO = true;
            // Process .nro file
            const char* version = getmeta(filePath);
            const char* name = getmetaname(filePath);

            // Display the file when "A" button is held
            PadState pad;
            padInitializeDefault(&pad);

            while (appletMainLoop()) {
                padUpdate(&pad);

                u64 kHeld = padGetButtons(&pad);
                if (kHeld & HidNpadButton_A) {
                    printf("%s - %s - %s\n", name, entry->d_name, version);
                    consoleUpdate(NULL);
                    break; // Proceed to the next file
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
    printf("Hold button A to list installed apps\n\n");

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

    // Track if any .nro files were found
    bool foundNRO = false;

    // Start scanning for .nro files
    scanForNROs(basePath, 0, &foundNRO);

    if (foundNRO) {
        printf("All files and subfolders have been scanned. Press PLUS to exit.\n");
        consoleUpdate(NULL);
    }

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
