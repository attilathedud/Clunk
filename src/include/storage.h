#ifndef STORAGE_H
#define STORAGE_H

typedef struct {
    int file_count;
    char *files[64];    //todo: fix 64 limit
} Storage;

int get_notes_in_directory(Storage *);
void storage_cleanup(Storage *);

#endif