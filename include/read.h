#define NRO_MAGIC "NRO0"
#define ASET_MAGIC "ASET"
#define TOOLBOX_FILE "toolbox.json"
#define FLAGS_DIR "flags"
#define BOOT2_FLAG "boot2.flag"
#define PREFIX "0100"

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

bool fileExists(const char* path);

void removeCR(char* json);
char* extractValueForKey(const char* json, const char* key);
void loadAsset(Asset *asset, uint8_t *data, size_t size);
int loadBinaryData(Editor *editor);
int checkStarFile(const char *dirpath, const char *filename);
double getFileSize(const char *file_path);
long getDirectorySize(const char* path);