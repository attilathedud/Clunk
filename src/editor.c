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

    buffer_cleanup(&(e->b));

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

void editor_print( Editor *e ) {
    if( e == NULL )
        return;

    node_t *iter = e->b.head;
    while( iter != NULL ) {
        if( iter->text != NULL ) {
            mvprintw( 0, NOTES_OFFSET, iter->text );
        }

        clrtoeol();
        iter = iter->next;
    }
}