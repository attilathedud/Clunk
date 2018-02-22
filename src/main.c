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

    mvvline( 0, MENU_OFFSET, ACS_VLINE, index * 2 );
    mvvline((index * 2) + 3, MENU_OFFSET, ACS_VLINE, LINES - (index * 2) - 1);
}

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
    int cur_selected_file_index = 0;
    int note_selection_scroll_offset = 0;

    Storage s = { 0 };

    if( get_notes_in_directory(&s) == -1 ) {
         fprintf( stderr, "Error getting the notes directory" );
    }

    initscr( );
    noecho( );
    cbreak( );
    keypad( stdscr, TRUE );

    start_color();
	init_pair(1, COLOR_BLACK, COLOR_CYAN);
    use_default_colors();

    do {
        erase();

        switch( ch ) {            
            case KEY_F(1):
                cur_selected_file_index--;
                if( cur_selected_file_index < 0 ) {
                    cur_selected_file_index = s.file_count - 1;
                    note_selection_scroll_offset = s.file_count + 1 - LINES / 2;
                    if( note_selection_scroll_offset < 0 ) {
                        note_selection_scroll_offset = 0;
                    }
                }

                if( cur_selected_file_index - note_selection_scroll_offset < 0 ) {
                    note_selection_scroll_offset--;
                }
                break;
            case KEY_F(2):
                cur_selected_file_index++;
                if( cur_selected_file_index > s.file_count - 1 ) {
                    cur_selected_file_index = 0;
                    note_selection_scroll_offset = 0;
                }

                if( ( cur_selected_file_index - note_selection_scroll_offset + 1 ) * 2 > LINES - 1 ) {
                    note_selection_scroll_offset++;
                }
                break;
            case KEY_F(5):
                create_note(&s);
                storage_cleanup(&s);
                get_notes_in_directory(&s);
                break;
            case KEY_F(8):
                //todo add confirmation dialog
                if( cur_selected_file_index < 0 || cur_selected_file_index > s.file_count - 1 )
                    break;

                delete_note(&s, cur_selected_file_index);
                storage_cleanup(&s);
                get_notes_in_directory(&s);

                if( cur_selected_file_index > s.file_count - 1 ) {
                    cur_selected_file_index = ( s.file_count - 1 < 0 ) ? 0 : s.file_count - 1;
                }

                if( ( cur_selected_file_index - note_selection_scroll_offset + 1 ) * 2 < LINES - 2 ) {
                    note_selection_scroll_offset--;
                }

                if( note_selection_scroll_offset < 0 ) {
                    note_selection_scroll_offset = 0;
                }
                break;
        }

        for( int i = 0; i + note_selection_scroll_offset < s.file_count; i++ ) {
            mvprintw((i * 2) + 1, 1, s.files[ i + note_selection_scroll_offset ]);
        }

        if( s.file_count > 0 && cur_selected_file_index < s.file_count) {
            print_file_selector(cur_selected_file_index - note_selection_scroll_offset);
        }
        else {
            mvvline( 0, MENU_OFFSET, ACS_VLINE, LINES - 1 );
        }

        print_help_line();

        move( 0, NOTES_OFFSET );
    } while( ( ch = getch( ) ) != KEY_F(10) );

    storage_cleanup(&s);

    refresh( );
    endwin( );

    return 0;
}