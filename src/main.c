#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

void clearselected(int selected) {
    if (selected == 1) {
        printf(CONSOLE_ESC(3;1H) CONSOLE_ESC(38;5;241m) "List aplications                                                                " CONSOLE_ESC(0m));
    } else if (selected == 2) {
        printf(CONSOLE_ESC(4;1H) CONSOLE_ESC(38;5;241m) "List overlays                                                                   " CONSOLE_ESC(0m));
    } else if (selected == 3) {
        printf(CONSOLE_ESC(5;1H) CONSOLE_ESC(38;5;241m) "List sysmodules                                                                 " CONSOLE_ESC(0m));
    } else if (selected == 4) {
        printf(CONSOLE_ESC(6;1H) CONSOLE_ESC(38;5;241m) "List payloads                                                                   " CONSOLE_ESC(0m));
    } else if (selected == 5) {
        printf(CONSOLE_ESC(7;1H) CONSOLE_ESC(38;5;241m) "List exeFS patches                                                              " CONSOLE_ESC(0m));
    } else if (selected == 6) {
        printf(CONSOLE_ESC(9;1H) CONSOLE_ESC(38;5;241m) "Export all                                                                      " CONSOLE_ESC(0m));
    } else if (selected == 7) {
        printf(CONSOLE_ESC(11;1H) CONSOLE_ESC(38;5;241m) "Exit                                                                            " CONSOLE_ESC(0m));
    }
}
void drawselected(int selected) {
    if (selected == 1) {
        printf(CONSOLE_ESC(3;1H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List aplications                                                                " CONSOLE_ESC(0m));
    } else if (selected == 2) {
        printf(CONSOLE_ESC(4;1H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List overlays                                                                   " CONSOLE_ESC(0m));
    } else if (selected == 3) {
        printf(CONSOLE_ESC(5;1H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List sysmodules                                                                 " CONSOLE_ESC(0m));
    } else if (selected == 4) {
        printf(CONSOLE_ESC(6;1H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List payloads                                                                   " CONSOLE_ESC(0m));
    } else if (selected == 5) {
        printf(CONSOLE_ESC(7;1H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List exeFS patches                                                              " CONSOLE_ESC(0m));
    } else if (selected == 6) {
        printf(CONSOLE_ESC(9;1H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "Export all                                                                      " CONSOLE_ESC(0m));
    } else if (selected == 7) {
        printf(CONSOLE_ESC(11;1H) CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "Exit                                                                            " CONSOLE_ESC(0m));
    }
}
int main(int argc, char* argv[]){
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    int selected = 1;
    printf(CONSOLE_ESC(38;5;255m) "Select an option\n\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;255m) CONSOLE_ESC(48;5;19m) "List aplications                                                                " CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) "List overlays\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) "List sysmodules\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) "List payloads\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) "List exeFS patches\n\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) "Export all\n\n" CONSOLE_ESC(0m));
    printf(CONSOLE_ESC(38;5;241m) "Exit" CONSOLE_ESC(0m));
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
            if (selected != 7) {
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
                exportall();
            } else if (selected == 7) {
                break;
            }
        }
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return 0;
}