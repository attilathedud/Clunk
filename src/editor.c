#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#include "include/editor.h"
#include "include/consts.h"

//todo deal with resizing

void editor_cleanup( Editor *e ) {
    if( e == NULL )
        return;

    buffer_cleanup(&(e->b));

    e->x = NOTES_OFFSET;
    e->y = 0;
    e->scroll_offset = 0;
    e->x_page_offset = 0;
}

void editor_load_file( Editor *e, const char *home_directory, const char *file ) {
    FILE *f = NULL;
    char *line = NULL;
    size_t len = 0;

    if( e == NULL || file == NULL )
        return;

    char *file_path = calloc(1, strlen( home_directory ) + strlen("/") + strlen( file ) + 1);
    strcpy(file_path, home_directory);
    strcat(file_path, "/");
    strcat(file_path, file);

    editor_cleanup(e);

    f = fopen(file_path, "r");
    if( f == NULL )
        return;

    while( ( line = fgetln( f, &len ) ) != NULL ) {
        if( line[ len - 1 ] == '\n' ) {
            line[ len - 1 ] = 0;
        }
        buffer_append_line( &(e->b), line, len );
    }

    fclose(f);

    if( file_path != NULL)
        free( file_path );
}

void editor_handle_input( Editor *e, const int ch ) {
    if( e == NULL )
        return;

    // todo: clean up
    // todo: add support for home and end, page up/down
    // todo: fix bug with scrolling right on saved files
    switch( ch ) {
        case KEY_UP:
            if( e->y > 0 ) e->y--;
            else if( e->scroll_offset > 0 ) e->scroll_offset--;
            else {
                e->x = NOTES_OFFSET;
                e->x_page_offset = 0;
            }

            if( e->x + e->x_page_offset > buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET - 1 ) {
                e->x = buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET;
                e->x_page_offset = 0;
                while( e->x > COLS - 2 ) {
                    e->x_page_offset++;
                    e->x--;
                }
            }
            break;
        case KEY_DOWN:
            if( e->y < LINES - 2 ) e->y++;
            else e->scroll_offset++;

            if( e->x + e->x_page_offset > buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET - 1 ) {
                e->x = buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET;
                e->x_page_offset = 0;
                while( e->x > COLS - 2 ) {
                    e->x_page_offset++;
                    e->x--;
                }
            }
            break;
        case KEY_LEFT:
            if( e->x > NOTES_OFFSET ) e->x--;
            else if( e->x_page_offset > 0 ) e->x_page_offset--;
            else {
                if( e->y + e->scroll_offset > 0 ) {
                    e->x = buffer_get_text_len( &(e->b), e->scroll_offset + e->y - 1 ) + NOTES_OFFSET;
                    while( e->x > COLS - 2 ) {
                        e->x_page_offset++;
                        e->x--;
                    }
                }
                if( e->y > 0 ) e->y--;
                else if( e->scroll_offset > 0 ) e->scroll_offset--;
            }
            break;
        case KEY_RIGHT:
            if( e->x < COLS - 2 ) e->x++;
            else e->x_page_offset++;

            if( e->x + e->x_page_offset > buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET) {
                e->x = NOTES_OFFSET;
                e->x_page_offset = 0;
                if( e->y < LINES - 2 ) e->y++;
                else e->scroll_offset++; 
            }
            break;
        case KEY_DELETE:
            if( e->x + e->x_page_offset == NOTES_OFFSET && e->y == 0 ) 
                break;
            
            if( e->x + e->x_page_offset > NOTES_OFFSET ) {
                buffer_remove_character( &(e->b), e->x - NOTES_OFFSET - 4 + e->x_page_offset, e->y + e->scroll_offset );
                if( e->x > NOTES_OFFSET ) e->x--;
                else if( e->x_page_offset > 0 ) e->x_page_offset--;
            }
            else {
                e->x = buffer_get_text_len( &(e->b), e->scroll_offset + e->y - 1 ) + NOTES_OFFSET;
                e->x_page_offset = 0;
                while( e->x > COLS - 2 ) {
                    e->x_page_offset++;
                    e->x--;
                }
                buffer_remove_line( &(e->b), e->y + e->scroll_offset );
                if( e->y > 0 ) e->y--;
                else if( e->scroll_offset > 0 ) e->scroll_offset--;
            }
            break;
        case KEY_TAB:
            for( int i = 0; i < 4; i++ ) {
                buffer_insert_character( &(e->b), ' ', e->x - NOTES_OFFSET - 4 + e->x_page_offset + i, e->y + e->scroll_offset );
            }
            e->x += 4;
            while( e->x > COLS - 2 ) {
                e->x_page_offset++;
                e->x--;
            }
            break;
        case KEY_RETURN:
            buffer_split_line(&(e->b), e->x - NOTES_OFFSET - 4 + e->x_page_offset, e->y + e->scroll_offset );
            if( e->y < LINES - 2 ) e->y++;
            else e->scroll_offset++;
            e->x = NOTES_OFFSET;
            e->x_page_offset = 0;
            break;
        default:
            if( isalnum( ch ) || isspace( ch ) ) { 
                buffer_insert_character( &(e->b), ch, e->x - NOTES_OFFSET - 4 + e->x_page_offset, e->y + e->scroll_offset );
                if( e->x < COLS - 2 ) e->x++;
                else e->x_page_offset++;
            }
            break;
    }

    if( e->x < NOTES_OFFSET ) e->x = NOTES_OFFSET;
}

char *editor_get_text( Editor *e ) {
    char *text = calloc( LINE_ALLOC_STEP * 10, sizeof( char ) );
    int allocs = 1;

    if( e == NULL )
        return NULL;
    
    node_t *iter = e->b.head;
    while( iter != NULL ) {
        if( strlen( text ) + ( iter->text == NULL ? 1 : strlen( iter->text ) ) + 1 > LINE_ALLOC_STEP * 10 * allocs ) {
            char *temp_buffer = text;
            allocs++;
            text = calloc( LINE_ALLOC_STEP * 10, sizeof( char ) );
            memcpy( text, temp_buffer, strlen( temp_buffer ));
            free( temp_buffer );
            temp_buffer = NULL;
        }

        if( iter->text == NULL ) {
            strcat( text, "\n" );
        }
        else {
            strcat( text, iter->text );
        }

        iter = iter->next;
    }

    return text;
}

void editor_print( Editor *e ) {
    int output_line = 0;

    if( e == NULL )
        return;

    node_t *iter = e->b.head;

    for( int i = 0; i < e->scroll_offset && iter->next != NULL; i++ ) {
        iter = iter->next;
    }

    char *buffer = calloc( COLS, sizeof( char ));

    // todo: only print text on screen
    while( iter != NULL ) {
        if( iter->text != NULL ) {
            memset(buffer, 0, COLS * sizeof(char) );
            strncpy( buffer, iter->text + e->x_page_offset, COLS - 1 - NOTES_OFFSET - 4 );
            buffer[COLS - 1 - NOTES_OFFSET - 4] = '\0';
            mvprintw(output_line, NOTES_OFFSET, buffer);
        }
        clrtoeol();

        output_line++;
        iter = iter->next;
    }

    free( buffer );
}

void editor_print_cursor( Editor *e ) {
    if( e == NULL )
        return;
    
    move(e->y, e->x);
}
