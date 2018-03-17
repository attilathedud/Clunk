#include <ncurses.h>

#include "include/consts.h"
#include "include/storage.h"
#include "include/menu.h"

int menu_init( Menu *m ) {
    if( m == NULL )
        return -1;

    if( storage_get_notes(&(m->s)) == -1 ) {
        return -1;
    }

    if( m->s.file_count == 0 ) {
        storage_create_note(&(m->s));
        storage_cleanup(&(m->s));
        storage_get_notes(&(m->s));
    }

    m->has_changed_file = true;

    return 0;
}

int menu_handle_input( Menu *m, const int ch ) {
    int pass_input_to_editor = false;

    if( m == NULL )
        return -1;

    switch( ch ) {
        case KEY_F(1):
            m->selected_file_index--;
            if( m->selected_file_index < 0 ) {
                m->selected_file_index = m->s.file_count - 1;
                m->scroll_offset = m->s.file_count + 1 - LINES / 2;
                if( m->scroll_offset < 0 ) {
                    m->scroll_offset = 0;
                }
            }

            if( m->selected_file_index - m->scroll_offset < 0 ) {
                m->scroll_offset--;
            }

            m->has_changed_file = true;

            break;
        case KEY_F(2):
            m->selected_file_index++;
            if( m->selected_file_index > m->s.file_count - 1 ) {
                m->selected_file_index = 0;
                m->scroll_offset = 0;
            }

            if( ( m->selected_file_index - m->scroll_offset + 1 ) * 2 > LINES - 1 ) {
                m->scroll_offset++;
            }

             m->has_changed_file = true;

            break;
        case KEY_F(6):
            storage_create_note(&(m->s));
            storage_cleanup(&(m->s));
            storage_get_notes(&(m->s));
            break;
        case KEY_F(8):
            if( m->s.file_count > 0 ) {
                m->is_deleting_file = true;
            }
            break;
        case KEY_LOWER_Y:
        case KEY_UPPER_Y:
            if( !m->is_deleting_file ) {
                pass_input_to_editor = true;
                break;
            }

            if( m->selected_file_index < 0 || m->selected_file_index > m->s.file_count - 1 )
                break;

            storage_delete_note(&(m->s), m->selected_file_index);
            storage_cleanup(&(m->s));
            storage_get_notes(&(m->s));

            if( m->selected_file_index > m->s.file_count - 1 ) {
                m->selected_file_index = ( m->s.file_count - 1 < 0 ) ? 0 : m->s.file_count - 1;
            }

            if( ( m->selected_file_index - m->scroll_offset + 1 ) * 2 < LINES - 2 ) {
                m->scroll_offset--;
            }

            if( m->scroll_offset < 0 ) {
                m->scroll_offset = 0;
            }

            m->has_changed_file = true;
            m->is_deleting_file = false;
            break;
        case KEY_LOWER_N:
        case KEY_UPPER_N:
            if( !m->is_deleting_file ) {
                pass_input_to_editor = true;
            }
            m->is_deleting_file = false;
            m->has_changed_file = false;
            break;
        default:
            m->is_deleting_file = false;
            m->has_changed_file = false;
            pass_input_to_editor = true;
            break;
    }

    return pass_input_to_editor;
}

void menu_save_note( Menu *m, const char *text ) {
    if( m == NULL || text == NULL )
        return;

    storage_save_note( &(m->s), m->selected_file_index, text );
}

void menu_print( Menu *m ) {
    for( int i = 0; i + m->scroll_offset < m->s.file_count; i++ ) {
        if( i + m->scroll_offset == m->selected_file_index ) {
            attron(COLOR_PAIR(1));
            move((i * 2) + 1, 5);
            printw(" ");
            printw(m->s.files[ i + m->scroll_offset ]);
            printw(" ");
            attroff(COLOR_PAIR(1));
        }
        else {
            mvprintw((i * 2) + 1, 1, m->s.files[ i + m->scroll_offset ]);
        }
    }

    if( m->is_deleting_file ) {
        attron(COLOR_PAIR(1));
        mvprintw( LINES - 2, 0, "Are you sure? (y/n):" );
        attroff(COLOR_PAIR(1));
    }
}

void menu_cleanup( Menu *m ) {
    storage_cleanup(&(m->s));
}
