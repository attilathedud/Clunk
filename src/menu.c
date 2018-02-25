#include <ncurses.h>

#include "include/menu.h"
#include "include/storage.h"

static Storage s = { 0 };

static int cur_selected_file_index = 0;
static int note_selection_scroll_offset = 0;
static int is_deleting_file = 0;

int init_menu() {
    if( get_notes_in_directory(&s) == -1 ) {
        return -1;
    }

    if( s.file_count == 0 ) {
        create_note(&s);
        storage_cleanup(&s);
        get_notes_in_directory(&s);
    }

    return 0;
}

void menu_handle_input( const int ch ) {
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
            if( s.file_count > 0 ) {
                is_deleting_file = true;
            }
            break;
        case KEY_LOWER_Y:
        case KEY_UPPER_Y:
            if( is_deleting_file ) {
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
            }
            is_deleting_file = false;
            break;
        default:
            is_deleting_file = false;
            break;
    }
}

void print_menu() {
    for( int i = 0; i + note_selection_scroll_offset < s.file_count; i++ ) {
        if( i + note_selection_scroll_offset == cur_selected_file_index ) {
            attron(COLOR_PAIR(1));
            move((i * 2) + 1, 5);
            printw(" ");
            printw(s.files[ i + note_selection_scroll_offset ]);
            printw(" ");
            attroff(COLOR_PAIR(1));
        }
        else {
            mvprintw((i * 2) + 1, 1, s.files[ i + note_selection_scroll_offset ]);
        }
    }

    if( is_deleting_file ) {
        attron(COLOR_PAIR(1));
        mvprintw( LINES - 2, 0, "Are you sure? (y/n):" );
        attroff(COLOR_PAIR(1));
    }
}

void menu_cleanup() {
    storage_cleanup(&s);
}
