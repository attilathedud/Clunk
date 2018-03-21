#include <string.h>
#include <stdlib.h>
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
    m->display_about_screen = false;

    return 0;
}

int menu_handle_input( Menu *m, Editor *e, const int ch ) {
    int pass_input_to_editor = false;
    char *note_text = NULL;

    if( m == NULL )
        return -1;

    m->display_about_screen = false;

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
        case KEY_F(5):
            note_text = editor_get_text(e);
            menu_save_note(m, note_text);
            free(note_text);
            attron(COLOR_PAIR(1));
            mvprintw( LINES - 2, 0, "Note saved" );
            attroff(COLOR_PAIR(1));
            break;
        case KEY_F(6):
            storage_create_note(&(m->s));
            storage_cleanup(&(m->s));
            storage_get_notes(&(m->s));
            break;
        case KEY_F(7):
            if( m->s.file_count > 0 ) {
                m->is_renaming_file = true;
                m->is_deleting_file = false;
                memset( m->rename_buffer, 0, sizeof( m->rename_buffer ));
            }
            break;
        case KEY_F(8):
            if( m->s.file_count > 0 ) {
                m->is_deleting_file = true;
                m->is_renaming_file = false;
            }
            break;
        case KEY_F(9):
            m->display_about_screen = true;
            break;
        case KEY_LOWER_Y:
        case KEY_UPPER_Y:
            if( !m->is_deleting_file && !m->is_renaming_file) {
                pass_input_to_editor = true;
                break;
            }

            if( m->is_renaming_file ) {
                if( strlen( m->rename_buffer ) < 128 ) {
                    m->rename_buffer[ strlen(m->rename_buffer) ] = ch;
                }
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
            if( !m->is_deleting_file && !m->is_renaming_file ) {
                pass_input_to_editor = true;
            }
            m->is_deleting_file = false;
            m->has_changed_file = false;

            if( m->is_renaming_file ) {
                if( strlen( m->rename_buffer ) < 128 ) {
                    m->rename_buffer[ strlen(m->rename_buffer) ] = ch;
                }
            }
            break;
        case KEY_RETURN:
            if( !m->is_renaming_file ) {
                pass_input_to_editor = true;
                break;
            }

            note_text = editor_get_text(e);
            menu_save_note(m, note_text);
            free(note_text);

            storage_rename_note(&(m->s), m->selected_file_index, m->rename_buffer );
            storage_cleanup(&(m->s));
            storage_get_notes(&(m->s));

            m->is_renaming_file = false;
            break;
        default:
            if( m->is_renaming_file ) {
                if( strlen( m->rename_buffer ) < 128 ) {
                    m->rename_buffer[ strlen(m->rename_buffer) ] = ch;
                }
            }
            else {
                m->is_deleting_file = false;
                m->is_renaming_file = false;
                m->has_changed_file = false;
                pass_input_to_editor = true;
            }
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
    // todo: add more about text
    if( m->display_about_screen ) {
        mvprintw( 0, COLS/ 2, "##### #     #   # #   # #  #");
        mvprintw( 1, COLS/ 2, "#     #     #   # # # # # #" );
        mvprintw( 2, COLS/ 2, "#     #     #   # #  ## #"   );
        mvprintw( 3, COLS/ 2, "#     #     #   # #   # # #" );
        mvprintw( 4, COLS/ 2, "##### ##### ##### #   # #  #");
        mvprintw( 5, COLS/ 2, "      Just take notes."      );

        mvprintw( 7, NOTES_OFFSET, "Clunk is a note taking application the stores notes locally.");
    }
    else {
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
        else if( m->is_renaming_file ) {
            attron(COLOR_PAIR(1));
            mvprintw( LINES - 2, 0, "Name (enter): " );
            printw( m->rename_buffer );
            attroff(COLOR_PAIR(1));
        }
    }
}

void menu_cleanup( Menu *m ) {
    storage_cleanup(&(m->s));
}
