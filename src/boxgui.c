#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include "boxgui.h"

void drawBox() {
    printf(CONSOLE_ESC(1;1H));
    printf("%c", (char) 218);
    for (int i = 0; i < 78; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 191);
    for (int i = 0; i < 43; i++) {
        printf("%c\n", (char) 179);
    }
    printf("%c", (char) 192);
    for (int i = 0; i < 78; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 217);
    printf(CONSOLE_ESC(2;1H));
    for (int i = 0; i < 43; i++) {
        printf(CONSOLE_ESC(79C));
        printf("%c", (char) 179);
        printf(CONSOLE_ESC(1D));
        printf("\n");
    }

    printf(CONSOLE_ESC(2;1H));
}