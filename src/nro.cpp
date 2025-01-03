#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include "nro.hpp"

#define NRO_MAGIC "NRO0"
#define ASET_MAGIC "ASET"

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

const char* getmetaversion(const char* path) {
    const char* NRO_FILE_PATH = path;
    Editor editor = {0};
    editor.filename = (char *)NRO_FILE_PATH;

    if (loadBinaryData(&editor)) {
        Asset *asset = &editor.asset;
        char* versionCopy = strdup(asset->version);
        free(asset->nacp);
        free(asset->icon);
        free(asset->romfs);
        free(editor.data);
        return versionCopy;
    }
    return "Failed to get";
}
const char* getmetaname(const char* path) {
    const char* NRO_FILE_PATH = path;
    Editor editor = {0};
    editor.filename = (char *)NRO_FILE_PATH;

    if (loadBinaryData(&editor)) {
        Asset *asset = &editor.asset;
        char* versionCopy = strdup(asset->name);
        free(asset->nacp);
        free(asset->icon);
        free(asset->romfs);
        free(editor.data);
        return versionCopy;
    }
    return "Failed to get";
}
const char* getmetaauthor(const char* path) {
    const char* NRO_FILE_PATH = path;
    Editor editor = {0};
    editor.filename = (char *)NRO_FILE_PATH;

    if (loadBinaryData(&editor)) {
        Asset *asset = &editor.asset;
        char* versionCopy = strdup(asset->author);
        free(asset->nacp);
        free(asset->icon);
        free(asset->romfs);
        free(editor.data);
        return versionCopy;
    }
    return "Failed to get";
}
