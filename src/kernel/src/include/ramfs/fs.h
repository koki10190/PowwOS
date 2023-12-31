#ifndef RAMFS_H
#define RAMFS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _file_t {
    char path[128];
    uint8_t *buffer;
    bool deleted;
} file_t;
#define MAX_RAMFS_FILES 500
typedef struct __fs_t {
    file_t files[MAX_RAMFS_FILES];
    int file_length;
} fs_t;

extern fs_t ramfs;

void init_ramfs();

file_t *ramfs_open(const char *path);
file_t *ramfs_open(const char *path);
file_t *ramfs_create(char *path, uint8_t *buffer);
void ramfs_delete(char *path);

#endif