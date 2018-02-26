#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "include/editor.h"
#include "include/consts.h"

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
    if( f == NULL ) {
        return;
    }

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

    switch( ch ) {
        case KEY_UP:
            if( e->y > 0 ) {
                e->y--;
            }
            break;
        case KEY_DOWN:
            if( e->y < LINES - 2 ) {
                e->y++;
            }
            break;
        case KEY_LEFT:
            if( e->x > NOTES_OFFSET ) {
                e->x--;
            }
            break;
        case KEY_RIGHT:
            if( e->x < COLS - 1) {
                e->x++;
            }
            break;
    }
}

void editor_print( Editor *e ) {
    int output_line = 0;

    if( e == NULL )
        return;

    node_t *iter = e->b.head;

    for( int i = 0; i < e->scroll_offset && iter->next != NULL; i++ )
    {
        iter = iter->next;
    }

    while( iter != NULL ) {
        if( iter->text != NULL ) {
            mvprintw( output_line, NOTES_OFFSET, iter->text );
        }

        clrtoeol();

        output_line++;
        iter = iter->next;
    }
}

void editor_print_cursor( Editor *e ) {
    if( e == NULL )
        return;

    move(e->y, e->x);
}
