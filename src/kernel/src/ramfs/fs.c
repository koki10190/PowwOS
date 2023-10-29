#include <ramfs/fs.h>
#include <string.h>
#include <panic/panic.h>
#include <graphics/graphics.h>
#include <uart.h>
#include <memory.h>

fs_t ramfs;

void init_ramfs() {
    ramfs.file_length = 0;

    for (int i = 0; i < MAX_RAMFS_FILES; i++) {
        ramfs.files[i].deleted = true;
    }
}

file_t *ramfs_open(const char *path) {
    for (int i = 0; i < MAX_RAMFS_FILES; i++) {
        if (!strcmp(ramfs.files[i].path, path) && !ramfs.files[i].deleted) {
            return &ramfs.files[i];
        }
    }

    return NULL;
}

file_t *ramfs_create(char *path, uint8_t *buffer) {
    file_t file;
    file.buffer = (uint8_t *)"";
    strcpy(file.path, path);
    file.deleted = false;

    ramfs.files[ramfs.file_length++] = file;
    return &ramfs.files[ramfs.file_length - 1];
}

void ramfs_delete(char *path) {
    file_t *file = ramfs_open(path);
    file->deleted = true;
    file->buffer = NULL;
    __memset(file->path, '\0', 128);
    ramfs.file_length--;
}