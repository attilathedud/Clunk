#ifndef STORAGE_H
#define STORAGE_H

#define NOTES_ALLOC_STEP 64

typedef struct {
    int file_count;
    int allocs;
    char **files;
    char *home_directory;
} Storage;

int storage_get_notes(Storage *);
void storage_cleanup(Storage *);
int storage_create_note(const Storage *);
int storage_delete_note(const Storage *, const int);
void storage_save_note( const Storage *, const int, const char * );
void storage_rename_note( const Storage *, const int, const char *);

#endif
