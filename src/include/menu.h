#ifndef MENU_H
#define MENU_H

    #include "storage.h"

    typedef struct {
        Storage s;
        int selected_file_index;
        int scroll_offset;
        int is_deleting_file;
    } Menu;

    int init_menu( Menu * );
    void menu_handle_input( Menu *, const int );
    void print_menu( Menu * );
    void menu_cleanup( Menu * );

#endif
