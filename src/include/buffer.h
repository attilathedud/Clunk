#ifndef BUFFER_H
#define BUFFER_H

typedef struct node {
    char *text;
    struct node *next;
} node_t;

typedef struct {
    node_t *head;
    node_t *current;
} Buffer;

void buffer_cleanup( Buffer * );
void buffer_append_line( Buffer *, const char *, const size_t );

#endif
