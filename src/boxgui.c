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

    printf(CONSOLE_ESC(3;1H));
}

void drawDetailsBox() {
    printf(CONSOLE_ESC(16;20H) CONSOLE_ESC(48;5;236m));
    printf("%c", (char) 218);
    for (int i = 0; i < 40; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 191);
    for (int i = 0; i < 13; i++) {
        printf("\n"CONSOLE_ESC(19C));
        printf("%c", (char) 179);
    }
    printf(CONSOLE_ESC(1D));
    printf("%c", (char) 192);
    for (int i = 0; i < 40; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 217);
    printf(CONSOLE_ESC(17;1H));
    for (int i = 0; i < 12; i++) {
        printf(CONSOLE_ESC(60C));
        printf("%c", (char) 179);
        printf(CONSOLE_ESC(1D));
        printf("\n");
    }
    printf(CONSOLE_ESC(17;1H));
    for (int i = 0; i < 12; i++) {
        printf(CONSOLE_ESC(20C));
        printf("                                        \n");
    }
}
void drawTop() {
    printf(CONSOLE_ESC(5;1H));
    printf("%c", (char) 195);
    for (int i = 0; i < 78; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 180);
}
void drawBottom() {
    printf(CONSOLE_ESC(41;1H));
    printf("%c", (char) 195);
    for (int i = 0; i < 78; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 180);
}
void drawFirstLine() {
    printf(CONSOLE_ESC(5;1H));
    printf("%c", (char) 195);
    for (int i = 0; i < 31; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 194);
    for (int i = 0; i < 25; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 194);
    for (int i = 0; i < 20; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 180);
}
void drawLastLine() {
    printf(CONSOLE_ESC(41;1H));
    printf("%c", (char) 195);
    for (int i = 0; i < 31; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 193);
    for (int i = 0; i < 25; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 193);
    for (int i = 0; i < 20; i++) {
        printf("%c", (char) 196);
    }
    printf("%c", (char) 180);
}