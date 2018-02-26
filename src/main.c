#include <ncurses.h>
#include <stdio.h>

#include "include/consts.h"
#include "include/menu.h"
#include "include/editor.h"

static void print_help_line() {
    move( LINES - 1, 0 );
    attron(COLOR_PAIR(1));
    printw("F1");
    attroff(COLOR_PAIR(1));
    printw("Prev Note ");
    attron(COLOR_PAIR(1));
    printw("F2");
    attroff(COLOR_PAIR(1));
    printw("Next Note ");
    attron(COLOR_PAIR(1));
    printw("F5");
    attroff(COLOR_PAIR(1));
    printw("Create Note ");
    attron(COLOR_PAIR(1));
    printw("F8");
    attroff(COLOR_PAIR(1));
    printw("Delete Note ");

    move(LINES - 1, COLS - 7);
    attron(COLOR_PAIR(1));
    printw("F10");
    attroff(COLOR_PAIR(1));
    printw("Exit");
}

int main( int argc, char** argv ) {
    int ch = 0;

    Menu m = { { 0 } };
    Editor e = { { 0 } };

    initscr( );
    noecho( );
    cbreak( );
    keypad( stdscr, TRUE );

    start_color();
	init_pair(1, COLOR_BLACK, COLOR_CYAN);
    use_default_colors();

    if( menu_init(&m) == -1 ) {
        refresh();
        endwin();
        fprintf( stderr, "Error getting the notes directory.\n" );
        return -1;
    }

    editor_load_file( &e, m.s.home_directory, m.s.files[ 0 ] );

    do {
        erase();

        // if the menu is controlled, don't pass input to editor
        if( menu_handle_input(&m, ch) == 1 ) {
            editor_handle_input(&e, ch);
        }

        if( m.has_changed_file ) {
            editor_load_file( &e, m.s.home_directory, m.s.files[ m.selected_file_index ] );
        }

        editor_print(&e);
        menu_print(&m);

        // print separator line
        mvvline( 0, MENU_OFFSET, ACS_VLINE, LINES - 1 );
        print_help_line();
        
        editor_print_cursor(&e);
    } while( ( ch = getch( ) ) != KEY_F(10) );

    menu_cleanup(&m);

    refresh( );
    endwin( );

    return 0;
}
