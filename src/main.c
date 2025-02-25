#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "boxgui.h"

void clearselected(int selected) {
    if (selected == 1) {
        printf(CONSOLE_ESC(8;2H) CONSOLE_ESC(38;5;241m) "List aplications                                                              \n" CONSOLE_ESC(0m));
    } else if (selected == 2) {
        printf(CONSOLE_ESC(9;2H) CONSOLE_ESC(38;5;241m) "List overlays                                                                 \n" CONSOLE_ESC(0m));
    } else if (selected == 3) {
        printf(CONSOLE_ESC(10;2H) CONSOLE_ESC(38;5;241m) "List sysmodules                                                               \n" CONSOLE_ESC(0m));
    } else if (selected == 4) {
        printf(CONSOLE_ESC(11;2H) CONSOLE_ESC(38;5;241m) "List payloads                                                                 \n" CONSOLE_ESC(0m));
    } else if (selected == 5) {
        printf(CONSOLE_ESC(12;2H) CONSOLE_ESC(38;5;241m) "List exeFS patches                                                            \n" CONSOLE_ESC(0m));
    } else if (selected == 6) {
        printf(CONSOLE_ESC(13;2H) CONSOLE_ESC(38;5;241m) "List external game content                                                    \n" CONSOLE_ESC(0m));
    } else if (selected == 7) {
        printf(CONSOLE_ESC(15;2H) CONSOLE_ESC(38;5;241m) "Export all                                                                    \n" CONSOLE_ESC(0m));
    } else if (selected == 8) {
        printf(CONSOLE_ESC(17;2H) CONSOLE_ESC(38;5;241m) "Exit                                                                          \n" CONSOLE_ESC(0m));
    }
}
void drawselected(int selected) {
    if (selected == 1) {
        printf(CONSOLE_ESC(8;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List aplications                                                              \n" CONSOLE_ESC(0m));
    } else if (selected == 2) {
        printf(CONSOLE_ESC(9;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List overlays                                                                 \n" CONSOLE_ESC(0m));
    } else if (selected == 3) {
        printf(CONSOLE_ESC(10;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List sysmodules                                                               \n" CONSOLE_ESC(0m));
    } else if (selected == 4) {
        printf(CONSOLE_ESC(11;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List payloads                                                                 \n" CONSOLE_ESC(0m));
    } else if (selected == 5) {
        printf(CONSOLE_ESC(12;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List exeFS patches                                                            \n" CONSOLE_ESC(0m));
    } else if (selected == 6) {
        printf(CONSOLE_ESC(13;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List external game content                                                    \n" CONSOLE_ESC(0m));
    } else if (selected == 7) {
        printf(CONSOLE_ESC(15;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "Export all                                                                    \n" CONSOLE_ESC(0m));
    } else if (selected == 8) {
        printf(CONSOLE_ESC(17;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "Exit                                                                          \n" CONSOLE_ESC(0m));
    }
}
void updateDetails() {
    time_t unixTime = time(NULL);
    struct tm* timeInfo = localtime(&unixTime);
    printf(CONSOLE_ESC(1;61H));
    printf("%02d:%02d:%02d %02d.%02d.%04d\n",
        timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec,
        timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year + 1900);
    psmInitialize();
    u32 batteryCharge;
    psmGetBatteryChargePercentage(&batteryCharge);
    printf(CONSOLE_ESC(45;76H));
    printf("%d%%", batteryCharge);
    if (batteryCharge <= 30 && batteryCharge > 0) {
        printf(CONSOLE_ESC(45;69H));
        printf("[");
        printf(CONSOLE_ESC(38;5;196m));
        printf("%c", (char) 176);
        printf(CONSOLE_ESC(0m)"  ]");
    } else if (batteryCharge <= 60 && batteryCharge > 30) {
        printf(CONSOLE_ESC(45;69H));
        printf("[");
        printf(CONSOLE_ESC(38;5;214m));
        printf("%c", (char) 177);
        printf("%c", (char) 177);
        printf(CONSOLE_ESC(0m)" ]");
    } else if (batteryCharge <= 100 && batteryCharge > 60) {
        printf(CONSOLE_ESC(45;69H));
        printf("[");
        printf(CONSOLE_ESC(38;5;28m));
        printf("%c", (char) 219);
        printf("%c", (char) 219);
        printf("%c", (char) 219);
        printf(CONSOLE_ESC(0m)"]");
    }
    PsmChargerType chargerType;
    psmGetChargerType(&chargerType);
    if (chargerType != PsmChargerType_Unconnected){
        printf(CONSOLE_ESC(45;74H) CONSOLE_ESC(38;5;28m));
        printf(" +"CONSOLE_ESC(0m));
    } else {
        printf(CONSOLE_ESC(45;74H) CONSOLE_ESC(38;5;196m));
        printf(" -"CONSOLE_ESC(0m));
    }
    psmExit();
    printf(CONSOLE_ESC(3;1H));
}
void printDetails() {
    printf(CONSOLE_ESC(1;2H));
    printf("NX-Homebrew-Lister 1.0.0");
    u32 version = hosversionGet();
    uint8_t major = HOSVER_MAJOR(version);
    uint8_t minor = HOSVER_MINOR(version);
    uint8_t micro = HOSVER_MICRO(version);
    printf(CONSOLE_ESC(45;2H));
    printf("%s%d%s%d%s%d", "HOS: ", (int)major, ".", (int)minor, ".", (int)micro);

    FsFileSystem *sdFs = fsdevGetDeviceFileSystem("sdmc");
    s64 totalSpaceBytes, freeSpaceBytes;
    fsFsGetTotalSpace(sdFs, "/", &totalSpaceBytes);
    fsFsGetFreeSpace(sdFs, "/", &freeSpaceBytes);
    double totalSpaceGB = (double)totalSpaceBytes / (1024 * 1024 * 1024);
    double freeSpaceGB = (double)freeSpaceBytes / (1024 * 1024 * 1024);
    printf(CONSOLE_ESC(45;15H));
    printf("SD:%.2f/%.2fGB", freeSpaceGB, totalSpaceGB);

    fsInitialize();
    FsFileSystem userFs;
    fsOpenBisFileSystem(&userFs, FsBisPartitionId_User, "");
    s64 totalSpaceBytes2, freeSpaceBytes2;
    fsFsGetTotalSpace(&userFs, "/", &totalSpaceBytes2);
    fsFsGetFreeSpace(&userFs, "/", &freeSpaceBytes2);
    double totalSpaceGB2 = (double)totalSpaceBytes2 / (1024 * 1024 * 1024);
    double freeSpaceGB2 = (double)freeSpaceBytes2 / (1024 * 1024 * 1024);
    printf(CONSOLE_ESC(45;34H));
    printf("NAND:%.2f/%.2fGB", freeSpaceGB2, totalSpaceGB2);
    fsFsClose(&userFs);

    printf(CONSOLE_ESC(3;1H));
}
int main(){
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    int selected = 1;
    int returnvalue = 0;
    drawBox();
    drawTop();
    drawBottom();
    printf(CONSOLE_ESC(3;37H) "Main menu");
    printf(CONSOLE_ESC(43;22H) "Up/Down - Move | B - Exit | A - Select");
    AppletType appletType = appletGetAppletType();
    printf(CONSOLE_ESC(6;2H));
    switch (appletType) {
        case AppletType_Application:
            printf(CONSOLE_ESC(38;5;28m) "Running in Title Takeover mode.\n\n" CONSOLE_ESC(0m));
            break;
        case AppletType_LibraryApplet:
        case AppletType_OverlayApplet:
        case AppletType_SystemApplet:
            printf(CONSOLE_ESC(38;5;196m) "Running in applet mode! Scanning larger software will cause a crash!\n\n" CONSOLE_ESC(0m));
            break;
        default:
            printf(CONSOLE_ESC(38;5;214m)"Could not get applet mode!\n\n" CONSOLE_ESC(0m));
            break;
    }
    printf(CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) CONSOLE_ESC(1C) "List aplications                                                              \n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) CONSOLE_ESC(1C) "List overlays\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) CONSOLE_ESC(1C) "List sysmodules\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) CONSOLE_ESC(1C) "List payloads\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) CONSOLE_ESC(1C) "List exeFS patches\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) CONSOLE_ESC(1C) "List external game content\n\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) CONSOLE_ESC(1C) "Export all\n\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) CONSOLE_ESC(1C) "Exit" CONSOLE_ESC(0m));
    printDetails();
    while (appletMainLoop()){
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        updateDetails();
        if (kDown & HidNpadButton_Plus) {
            break;
        }
        if (kDown & HidNpadButton_AnyUp) {
            if (selected != 1) {
                clearselected(selected);
                selected = selected - 1;
                drawselected(selected);
            }
        }
        if (kDown & HidNpadButton_AnyDown) {
            if (selected != 8) {
                clearselected(selected);
                selected = selected + 1;
                drawselected(selected);
            } 
        }
        if (kDown &  HidNpadButton_A) {
            if (selected == 1) {
                returnvalue = listApps();
                break;
            } else if (selected == 2) {
                //List ovl
            } else if (selected == 3) {
                //List sys
            } else if (selected == 4) {
                //List payl
            } else if (selected == 5) {
                //List exefs
            } else if (selected == 6) {
                //List game mods 0x0100 or 00100*
            } else if (selected == 7) {
                returnvalue = exportAll();
                break;
            } else if (selected == 8) {
                break;
            }
        }
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    if (returnvalue == 1) {
        main();
    }
    return 0;
}