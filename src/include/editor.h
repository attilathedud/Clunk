#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

#define KEY_DELETE 127
#define KEY_TAB 9
#define KEY_RETURN 10
#define TEXT_OFFSET NOTES_OFFSET - 4

typedef struct {
    Buffer b;
    int x;
    int y;
    int scroll_offset;
    int x_page_offset;
} Editor;

void editor_cleanup( Editor * );
void editor_load_file( Editor *, const char *, const char * );
void editor_handle_input( Editor *, const int );
void editor_print( Editor * );
void editor_print_cursor( Editor * );
char *editor_get_text( Editor * );

#endif
