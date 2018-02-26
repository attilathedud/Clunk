#ifndef MENU_H
#define MENU_H

#include "storage.h"

typedef struct {
    Storage s;
    int selected_file_index;
    int scroll_offset;
    int is_deleting_file;
    int has_changed_file;
} Menu;

int menu_init( Menu * );
int menu_handle_input( Menu *, const int );
void menu_print( Menu * );
void menu_cleanup( Menu * );

#endif
