#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#define NRO_MAGIC "NRO0"
#define ASET_MAGIC "ASET"

typedef struct {
    uint8_t *nacp;
    uint8_t *icon;
    uint8_t *romfs;
    size_t offset;
    char name[512];
    char author[256];
    char version[16];
} Asset;

typedef struct {
    char *filename;
    uint8_t *data;
    size_t nrosize;
    Asset asset;
} Editor;

//Read .nro file assets
void loadAsset(Asset *asset, uint8_t *data, size_t size) {
    size_t offset = 0x8;
    uint64_t icon_pos = *(uint64_t *)(data + offset);
    uint64_t icon_size = *(uint64_t *)(data + offset + 0x8);
    asset->icon = (uint8_t *)malloc(icon_size);
    memcpy(asset->icon, data + icon_pos, icon_size);
    offset += 0x10;
    uint64_t nacp_pos = *(uint64_t *)(data + offset);
    uint64_t nacp_size = *(uint64_t *)(data + offset + 0x8);
    asset->nacp = (uint8_t *)malloc(nacp_size);
    memcpy(asset->nacp, data + nacp_pos, nacp_size);
    offset += 0x10;
    uint64_t romfs_pos = *(uint64_t *)(data + offset);
    uint64_t romfs_size = *(uint64_t *)(data + offset + 0x8);
    asset->romfs = (uint8_t *)malloc(romfs_size);
    memcpy(asset->romfs, data + romfs_pos, romfs_size);

    strncpy(asset->name, (char *)(asset->nacp), 512);
    asset->name[511] = '\0';
    strncpy(asset->author, (char *)(asset->nacp + 0x200), 256);
    asset->author[255] = '\0';
    strncpy(asset->version, (char *)(asset->nacp + 0x3060), 16);
    asset->version[15] = '\0';
}

//Read .nro file
int loadBinaryData(Editor *editor) { 
    FILE *file = fopen(editor->filename, "rb");
    if (!file) {
        fprintf(stderr, "[ERROR] Failed to open file: %s\n", editor->filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    editor->data = (uint8_t *)malloc(filesize);
    fread(editor->data, 1, filesize, file);
    fclose(file);

    uint8_t *data = editor->data;

    if (memcmp(data + 0x10, NRO_MAGIC, 4) != 0) {
        fprintf(stderr, "[ERROR] Invalid NRO file format.\n");
        free(editor->data);
        return 0;
    }
    editor->nrosize = *(uint32_t *)(data + 0x18);
    if (filesize > editor->nrosize + 4 && memcmp(data + editor->nrosize, ASET_MAGIC, 4) == 0) {
        loadAsset(&editor->asset, data + editor->nrosize, filesize - editor->nrosize);
    }

    return 1;
}

int main(int argc, char **argv) {
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    const char *NRO_FILE_PATH = "/switch/appstore.nro";

    Editor editor = {0};
    editor.filename = (char *)NRO_FILE_PATH;

    if (loadBinaryData(&editor)) {
        Asset *asset = &editor.asset;
        printf("\x1b[3;1HFile: %s\n", NRO_FILE_PATH);
        printf("\x1b[4;1HTitle: %s\n", asset->name);
        printf("\x1b[5;1HAuthor: %s\n", asset->author);
        printf("\x1b[6;1HVersion: %s\n", asset->version);

        free(asset->nacp);
        free(asset->icon);
        free(asset->romfs);
        free(editor.data);
    } else {
        printf("\x1b[3;1HFailed to read metadata from the NRO file.\n");
    }
    printf("\x1b[8;1HPress + to exit.\n");

    while (appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus)
            break;
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return 0;
}