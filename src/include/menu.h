#ifndef MENU_H
#define MENU_H

#include "storage.h"
#include "editor.h"

#define KEY_UPPER_Y 89
#define KEY_LOWER_Y 121
#define KEY_LOWER_N 110
#define KEY_UPPER_N 78
#define KEY_RETURN 10

typedef struct {
    Storage s;
    int selected_file_index;
    int scroll_offset;
    int is_deleting_file;
    int is_renaming_file;
    int has_changed_file;
    char rename_buffer[ 128 ];      //todo: explain why acceptable
} Menu;

int menu_init( Menu * );
int menu_handle_input( Menu *, Editor *, const int );
void menu_print( Menu * );
void menu_cleanup( Menu * );
void menu_save_note( Menu *, const char * );

#endif
