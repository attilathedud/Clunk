#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

#define KEY_TAB 9
#define KEY_RETURN 10
#define TEXT_OFFSET NOTES_OFFSET - 4

typedef struct {
    Buffer b;
    size_t x;
    size_t y;
    size_t scroll_offset;
    size_t x_page_offset;
    int is_modified;
} Editor;

void editor_cleanup( Editor * );
void editor_load_file( Editor *, const char *, const char * );
void editor_handle_input( Editor *, const int );
void editor_print( const Editor * );
void editor_print_cursor( const Editor * );
char *editor_get_text( Editor * );

#endif
