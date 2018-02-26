#ifndef STORAGE_H
#define STORAGE_H

#include "consts.h"

typedef struct {
    int file_count;
    char **files;
    char *home_directory;
} Storage;

int storage_get_notes(Storage *);
void storage_cleanup(Storage *);
int storage_create_note(const Storage *);
int storage_delete_note(const Storage *, const int);

#endif
