#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    switch( ch ) {
        case KEY_UP:
            if( e->y > 0 ) e->y--;
            else if( e->scroll_offset > 0 ) e->scroll_offset--;

            if( e->x > buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET - 1 ) {
                e->x = buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET - 1;
            }
            if( e->x < NOTES_OFFSET ) e->x = NOTES_OFFSET;
            break;
        case KEY_DOWN:
            if( e->y < LINES - 2 ) e->y++;
            else e->scroll_offset++;

            if( e->x > buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET - 1 ) {
                e->x = buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET - 1;
            }
            if( e->x < NOTES_OFFSET ) e->x = NOTES_OFFSET;
            break;
        case KEY_LEFT:
            // todo: fix bug where going to previous line doesn't set x correctly
            if( e->x > NOTES_OFFSET ) { e->x--; }
            else {
                if( e->y + e->scroll_offset > 0 ) {
                    e->x = buffer_get_text_len( &(e->b), e->scroll_offset + e->y - 1 ) + NOTES_OFFSET - 1;
                }
                if( e->x < NOTES_OFFSET ) e->x = NOTES_OFFSET;
                if( e->y > 0 ) e->y--;
                else if( e->scroll_offset > 0 ) e->scroll_offset--;
            }
            break;
        case KEY_RIGHT:
            if( e->x > COLS - 1 || e->x > buffer_get_text_len( &(e->b), e->scroll_offset + e->y ) + NOTES_OFFSET - 1) {
                e->x = NOTES_OFFSET;
                if( e->y < LINES - 2 ) e->y++;
                else e->scroll_offset++; 
            }
            else e->x++;
            break;
        case KEY_DELETE:
            if( e->x == NOTES_OFFSET && e->y == 0 ) 
                break;
            
            if( e->x > NOTES_OFFSET ) {
                buffer_remove_character( &(e->b), e->x - NOTES_OFFSET - 4, e->y + e->scroll_offset );
                e->x--;
            }
            break;
        default:
            // todo: fix insert on multiple lines
            buffer_insert_character( &(e->b), ch, e->x - NOTES_OFFSET - 4, e->y + e->scroll_offset );
            e->x++;
            break;
    }
}

void editor_print( Editor *e ) {
    int output_line = 0;
    int output_offset = 0;

    if( e == NULL )
        return;

    node_t *iter = e->b.head;

    for( int i = 0; i < e->scroll_offset && iter->next != NULL; i++ ) {
        iter = iter->next;
    }

    char *buffer = calloc( COLS, sizeof( char ));

    while( iter != NULL ) {
        if( iter->text != NULL ) {
            //todo fix magic numbers
            //todo fix cursor pos
            if( strlen(iter->text) > COLS - 1 - NOTES_OFFSET - 4 ) {
                for( int i = 0; i < strlen( iter->text ); i += COLS - 1 - NOTES_OFFSET  - 4, output_offset++ ) {
                    strncpy( buffer, iter->text + i, COLS - 1 - NOTES_OFFSET - 4 );
                    buffer[COLS - 1 - NOTES_OFFSET - 4] = '\0';
                    mvprintw(output_line + output_offset, NOTES_OFFSET, buffer);
                }
            }
            else {
                mvprintw( output_line, NOTES_OFFSET, iter->text );
            }
        }
        clrtoeol();

        output_line++;
        output_line += output_offset;
        iter = iter->next;
        output_offset = 0;
    }

    free( buffer );
}

void editor_print_cursor( Editor *e ) {
    if( e == NULL )
        return;

    move(e->y, e->x);
}
