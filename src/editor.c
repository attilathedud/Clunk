#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#include "include/editor.h"
#include "include/consts.h"

#define SET_X_TO_LINE_LENGTH(Y)     e->x = buffer_get_text_len( &(e->b), e->scroll_offset + Y ) + NOTES_OFFSET; \
                                    e->x_page_offset = 0; \
                                    while( e->x > COLS - 2 ) { \
                                        e->x_page_offset++; \
                                        e->x--; \
                                    }
                                
#define SET_X_TO_BEGINNING          e->x = NOTES_OFFSET; \
                                    e->x_page_offset = 0;

#define INCREASE_X                  if( e->x < COLS - 2 ) e->x++; \
                                    else e->x_page_offset++;

#define DECREASE_X                  if( e->x > NOTES_OFFSET ) e->x--; \
                                    else if( e->x_page_offset > 0 ) e->x_page_offset--;

#define DECREASE_Y                  if( e->y > 0 ) e->y--; \
                                    else if( e->scroll_offset > 0 ) e->scroll_offset--;

#define INCREASE_Y                  if( e->y < LINES - 2 ) e->y++; \
                                    else e->scroll_offset++;                                    

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

    // todo: add support for home/end, page up/down
    switch( ch ) {
        case KEY_UP:
            if( e->y == 0 && e->scroll_offset == 0 ) {
                SET_X_TO_BEGINNING
            }
            else DECREASE_Y
            
            if( e->x + e->x_page_offset > buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET )
                SET_X_TO_LINE_LENGTH(e->y)
            break;
        case KEY_DOWN:
            INCREASE_Y

            if( e->x + e->x_page_offset > buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET )
                SET_X_TO_LINE_LENGTH(e->y)
            break;
        case KEY_LEFT:
            if( e->x + e->x_page_offset == NOTES_OFFSET ) {
                if( e->y + e->scroll_offset > 0 )
                    SET_X_TO_LINE_LENGTH(e->y - 1)

                DECREASE_Y
            }
            else DECREASE_X
            break;
        case KEY_RIGHT:
            INCREASE_X

            if( e->x + e->x_page_offset > buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET) {
                SET_X_TO_BEGINNING
                INCREASE_Y
            }
            break;
        case KEY_DELETE:
            if( e->x + e->x_page_offset == NOTES_OFFSET && e->y == 0 ) 
                break;
            
            if( e->x + e->x_page_offset > NOTES_OFFSET ) {
                buffer_remove_character( &(e->b), e->x - TEXT_OFFSET + e->x_page_offset, e->y + e->scroll_offset );
                DECREASE_X
            }
            else {
                SET_X_TO_LINE_LENGTH(e->y - 1)
                buffer_remove_line( &(e->b), e->y + e->scroll_offset );
                DECREASE_Y
            }
            break;
        case KEY_TAB:
            for( int i = 0; i < 4; i++ ) {
                buffer_insert_character( &(e->b), ' ', e->x - TEXT_OFFSET + e->x_page_offset + i, e->y + e->scroll_offset );
            }
            e->x += 4;
            while( e->x > COLS - 2 ) {
                e->x_page_offset++;
                e->x--;
            }
            break;
        case KEY_RETURN:
            buffer_split_line(&(e->b), e->x - TEXT_OFFSET + e->x_page_offset, e->y + e->scroll_offset );
            INCREASE_Y
            SET_X_TO_BEGINNING
            break;
        default:
            if( !( isalnum( ch ) || isspace( ch )) )
                break;                
            buffer_insert_character( &(e->b), ch, e->x - TEXT_OFFSET + e->x_page_offset, e->y + e->scroll_offset );
            INCREASE_X
            break;
    }

    if( e->x < NOTES_OFFSET ) e->x = NOTES_OFFSET;
}

// todo: on newline create 'n'
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

    while( iter != NULL && output_line < LINES - 1) {
        if( iter->text != NULL ) {
            memset(buffer, 0, COLS * sizeof(char) );
            strncpy( buffer, iter->text + e->x_page_offset, COLS - 1 - TEXT_OFFSET );
            buffer[COLS - 1 - TEXT_OFFSET] = '\0';
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
