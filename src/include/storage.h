#ifndef STORAGE_H
#define STORAGE_H

#include "consts.h"

typedef struct {
    int file_count;
    char *files[MAX_NOTES];
    char *home_directory;
} Storage;

int get_notes_in_directory(Storage *);
void storage_cleanup(Storage *);
int create_note(const Storage *);
int delete_note(const Storage *, const int);

#endif