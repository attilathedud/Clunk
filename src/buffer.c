#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/buffer.h"

void buffer_cleanup( Buffer *b ) {
    if( b == NULL )
        return;

    node_t *previous = NULL;
    b->current = b->head;

    while( b->current != NULL )
    {
        if( previous != NULL )
            free( previous );

        if( b->current->text != NULL ) {
            free( b->current->text );
            b->current->text = NULL;
        }

        previous = b->current;
        b->current = b->current->next;
    }

    if( previous != NULL )
        free( previous );

    b->head = NULL;
    b->current = NULL;
}

void buffer_append_line( Buffer *b, const char *line, const size_t len ) {
    if( b == NULL ) 
        return;

    node_t *new_node = calloc( 1, sizeof( node_t ) );
    if( new_node == NULL )
        return;

    new_node->text = calloc( len + 1, sizeof( char ) );
    strncpy( new_node->text, line, len );

    if( b->head == NULL ) {
        b->head = new_node;
        b->current = b->head;
    }
    else {
        b->current->next = new_node;
        b->current = b->current->next;
    }
}
