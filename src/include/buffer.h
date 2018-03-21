#ifndef BUFFER_H
#define BUFFER_H

#define LINE_ALLOC_STEP 128

typedef struct node {
    char *text;
    size_t allocs;
    struct node *next;
} node_t;

typedef struct {
    node_t *head;
    node_t *current;
} Buffer;

void buffer_cleanup( Buffer * );
void buffer_append_line( Buffer *, const char *, const size_t );
size_t buffer_get_text_len( Buffer *, const size_t );
void buffer_insert_character( Buffer *, const char, const size_t, const size_t );
void buffer_remove_character( Buffer *, const size_t, const size_t );
void buffer_remove_line( Buffer *, const size_t );
void buffer_split_line( Buffer *, const size_t, const size_t );

#endif
