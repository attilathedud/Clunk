#include <ncurses.h>
#include <stdio.h>

#include "include/consts.h"
#include "include/menu.h"


/*
void print_file( int index ) {
    int c = 0;
    int col = 0;

    FILE *fptr = NULL;

    fptr = fopen(files[index], "r");
    if( fptr == NULL ) {
        return;
    }

    while((c = fgetc(fptr)) != EOF) {
        mvprintw( 4, col, &c );
        col++;
    }

    fclose(fptr);
}
*/

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

    initscr( );
    noecho( );
    cbreak( );
    keypad( stdscr, TRUE );

    start_color();
	init_pair(1, COLOR_BLACK, COLOR_CYAN);
    use_default_colors();

    if( init_menu() == -1 ) {
        refresh();
        endwin();
        fprintf( stderr, "Error getting the notes directory.\n" );
        return -1;
    }

    do {
        erase();

        menu_handle_input(ch);

        // print separator line
        mvvline( 0, MENU_OFFSET, ACS_VLINE, LINES - 1 );
        print_help_line();

        print_menu();

        move( 0, NOTES_OFFSET );
    } while( ( ch = getch( ) ) != KEY_F(10) );

    menu_cleanup();

    refresh( );
    endwin( );

    return 0;
}