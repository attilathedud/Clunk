#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/buffer.h"
#include "include/consts.h"

static void buffer_set_current_node( Buffer *b, const int index ) {
    if( b == NULL )
        return;

    if( b->head == NULL ) {
        b->head = calloc( 1, sizeof( node_t ) );
    }

    b->current = b->head;
    for( int i = 0; i < index; i++ ) {
        if( b->current->next == NULL ) {
            buffer_append_line( b, NULL, 0 );
        }
        else {
            b->current = b->current->next;
        }
    }
}

void buffer_cleanup( Buffer *b ) {
    node_t *previous = NULL;
    
    if( b == NULL )
        return;

    b->current = b->head;

    while( b->current != NULL ) {
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

size_t buffer_get_text_len( Buffer *b, const int index ) {
    if( b == NULL )
        return 0;

    buffer_set_current_node( b, index );
    if( b->current->text == NULL )
        return 0;

    return strlen(b->current->text);
}

void buffer_append_line( Buffer *b, const char *line, const size_t len ) {
    if( b == NULL ) 
        return;

    node_t *new_node = calloc( 1, sizeof( node_t ) );
    if( new_node == NULL )
        return;

    if( len > 0 && line != NULL ) {
        new_node->text = calloc( len + 1, sizeof( char ) );
        strncpy( new_node->text, line, len );
    }

    if( b->head == NULL ) {
        b->head = new_node;
        b->current = b->head;
    }
    else {
        b->current->next = new_node;
        b->current = b->current->next;
    }
}

void buffer_insert_character( Buffer *b, const char ch, const int x, const int index ) {
    if( b == NULL )
        return;

    buffer_set_current_node( b, index );
    if( b->current->text == NULL ) {
        b->current->text = calloc( LINE_ALLOC_STEP, sizeof( char ) );
        b->current->allocs++;
    }

    if( strlen( b->current->text ) + 2 > LINE_ALLOC_STEP * b->current->allocs ) {
        char *temp_buffer = b->current->text;
        b->current->allocs++;
        b->current->text = calloc(LINE_ALLOC_STEP * b->current->allocs, sizeof( char ));
        memcpy(b->current->text, temp_buffer, strlen(temp_buffer));
        free(temp_buffer);
        temp_buffer = NULL;
    }

    for( int i = 0; i < x; i++ ) {
        if( b->current->text[ i ] == 0 ) {
            b->current->text[ i ] = ' ';
        }
    }

    b->current->text[ x ] = ch;
}

void buffer_remove_character( Buffer *b, const int x, const int index ) {
    if( b == NULL )
        return;

    buffer_set_current_node( b, index );
    if( b->current->text == NULL )
        return;

    for( int i = x - 1; i < strlen( b->current->text ); i++ ) {
        b->current->text[ i ] = b->current->text[ i + 1 ];
    }

    b->current->text[ strlen( b->current->text ) ] = 0;
}
