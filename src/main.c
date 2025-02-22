#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "boxgui.h"

void clearselected(int selected) {
    if (selected == 1) {
        printf(CONSOLE_ESC(4;2H) CONSOLE_ESC(38;5;241m) "List aplications                                                              \n" CONSOLE_ESC(0m));
    } else if (selected == 2) {
        printf(CONSOLE_ESC(5;2H) CONSOLE_ESC(38;5;241m) "List overlays                                                                 \n" CONSOLE_ESC(0m));
    } else if (selected == 3) {
        printf(CONSOLE_ESC(6;2H) CONSOLE_ESC(38;5;241m) "List sysmodules                                                               \n" CONSOLE_ESC(0m));
    } else if (selected == 4) {
        printf(CONSOLE_ESC(7;2H) CONSOLE_ESC(38;5;241m) "List payloads                                                                 \n" CONSOLE_ESC(0m));
    } else if (selected == 5) {
        printf(CONSOLE_ESC(8;2H) CONSOLE_ESC(38;5;241m) "List exeFS patches                                                            \n" CONSOLE_ESC(0m));
    } else if (selected == 6) {
        printf(CONSOLE_ESC(9;2H) CONSOLE_ESC(38;5;241m) "List external game content                                                    \n" CONSOLE_ESC(0m));
    } else if (selected == 7) {
        printf(CONSOLE_ESC(11;2H) CONSOLE_ESC(38;5;241m) "Export all                                                                    \n" CONSOLE_ESC(0m));
    } else if (selected == 8) {
        printf(CONSOLE_ESC(13;2H) CONSOLE_ESC(38;5;241m) "Exit                                                                          \n" CONSOLE_ESC(0m));
    }
}
void drawselected(int selected) {
    if (selected == 1) {
        printf(CONSOLE_ESC(4;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List aplications                                                              \n" CONSOLE_ESC(0m));
    } else if (selected == 2) {
        printf(CONSOLE_ESC(5;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List overlays                                                                 \n" CONSOLE_ESC(0m));
    } else if (selected == 3) {
        printf(CONSOLE_ESC(6;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List sysmodules                                                               \n" CONSOLE_ESC(0m));
    } else if (selected == 4) {
        printf(CONSOLE_ESC(7;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List payloads                                                                 \n" CONSOLE_ESC(0m));
    } else if (selected == 5) {
        printf(CONSOLE_ESC(8;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List exeFS patches                                                            \n" CONSOLE_ESC(0m));
    } else if (selected == 6) {
        printf(CONSOLE_ESC(9;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List external game content                                                    \n" CONSOLE_ESC(0m));
    } else if (selected == 7) {
        printf(CONSOLE_ESC(11;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "Export all                                                                    \n" CONSOLE_ESC(0m));
    } else if (selected == 8) {
        printf(CONSOLE_ESC(13;2H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "Exit                                                                          \n" CONSOLE_ESC(0m));
    }
}
int main(int argc, char* argv[]){
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    int selected = 1;
    drawBox();
    AppletType appletType = appletGetAppletType();
    switch (appletType) {
        case AppletType_Application:
            printf(CONSOLE_ESC(38;5;28m) CONSOLE_ESC(1C) "Running in Title Takeover mode.\n\n" CONSOLE_ESC(0m));
            break;
        case AppletType_LibraryApplet:
        case AppletType_OverlayApplet:
        case AppletType_SystemApplet:
            printf(CONSOLE_ESC(38;5;196m) CONSOLE_ESC(1C) "Running in applet mode! Scanning larger software will cause a crash!\n\n" CONSOLE_ESC(0m));
            break;
        default:
            printf(CONSOLE_ESC(38;5;214m) CONSOLE_ESC(1C) "Could not get applet mode!\n\n" CONSOLE_ESC(0m));
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
    while (appletMainLoop()){
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
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
                //List apps
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
                exportall();
            } else if (selected == 8) {
                break;
            }
        }
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return 0;
}