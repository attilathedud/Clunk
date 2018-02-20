#include <ncurses.h>
#include <stdio.h>

#include "include/consts.h"
#include "include/storage.h"


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

static void print_file_selector( const int index ) {
    mvhline(index * 2, 0, ACS_HLINE, MENU_OFFSET );
    mvhline((index * 2) + 2, 0, ACS_HLINE, MENU_OFFSET );
}

int main( int argc, char** argv ) {
    int ch = 0;
    int cur_selected_file_index = 0;

    Storage s = { 0 };

    if( get_notes_in_directory(&s) == -1 ) {
         fprintf( stderr, "Error getting the notes directory" );
    }

    initscr( );
    noecho( );
    cbreak( );
    keypad( stdscr, TRUE );

    do {
        erase();

        switch( ch ) {
            case KEY_F(2):
                cur_selected_file_index++;
                if( cur_selected_file_index > s.file_count - 1 ) {
                    cur_selected_file_index = 0;
                }
                break;
            case KEY_F(1):
                cur_selected_file_index--;
                if( cur_selected_file_index < 0 ) {
                    cur_selected_file_index = s.file_count - 1;
                }
                break;
        }

        mvvline( 0, MENU_OFFSET, ACS_VLINE, LINES - 1 );

        for( int i = 0; i < s.file_count; i++ ) {
            mvprintw((i * 2) + 1, 1, s.files[ i ]);
        }

        if( s.file_count > 0 && cur_selected_file_index < s.file_count) {
            print_file_selector(cur_selected_file_index);
        }

        move( 0, NOTES_OFFSET );
    } while( ( ch = getch( ) ) != KEY_F(10) );


    storage_cleanup(&s);

    refresh( );
    endwin( );

    return 0;
}