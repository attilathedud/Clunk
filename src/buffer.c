#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/buffer.h"
#include "include/consts.h"

static void buffer_set_current_node(Buffer *b, const size_t index)
{
    if (b == NULL)
        return;

    if (b->head == NULL)
    {
        b->head = calloc(1, sizeof(node_t));
    }

    b->current = b->head;
    for (int i = 0; i < index; i++)
    {
        if (b->current->next == NULL)
        {
            buffer_append_line(b, NULL, 0);
        }
        else
        {
            b->current = b->current->next;
        }
    }
}

void buffer_cleanup(Buffer *b)
{
    node_t *previous = NULL;

    if (b == NULL)
        return;

    b->current = b->head;

    while (b->current != NULL)
    {
        if (previous != NULL)
            free(previous);

        if (b->current->text != NULL)
        {
            free(b->current->text);
            b->current->text = NULL;
        }

        previous = b->current;
        b->current = b->current->next;
    }

    if (previous != NULL)
        free(previous);

    b->head = NULL;
    b->current = NULL;
}

size_t buffer_get_text_len(Buffer *b, const size_t index)
{
    if (b == NULL)
        return 0;

    buffer_set_current_node(b, index);
    if (b->current->text == NULL)
        return 0;

    return strlen(b->current->text);
}

void buffer_append_line(Buffer *b, const char *line, const size_t len)
{
    if (b == NULL)
        return;

    node_t *new_node = calloc(1, sizeof(node_t));
    if (new_node == NULL)
        return;

    if (len > 0 && line != NULL)
    {
        new_node->text = calloc(len + 1, sizeof(char));
        for (new_node->allocs = 0; (new_node->allocs + 1) * LINE_ALLOC_STEP < len; new_node->allocs++)
            ;
        strncpy(new_node->text, line, len);
    }

    if (b->head == NULL)
    {
        b->head = new_node;
        b->current = b->head;
    }
    else
    {
        b->current->next = new_node;
        b->current = b->current->next;
    }
}

void buffer_split_line(Buffer *b, const size_t x, const size_t index)
{
    if (b == NULL)
        return;

    buffer_set_current_node(b, index);
    node_t *next_node = b->current->next;
    node_t *new_node = calloc(1, sizeof(node_t));
    if (new_node == NULL)
        return;

    new_node->text = calloc(LINE_ALLOC_STEP * (b->current->allocs + 1), sizeof(char));
    new_node->allocs = b->current->allocs;
    if (b->current->text != NULL)
    {
        memcpy(new_node->text, b->current->text + x, strlen(b->current->text) - x);
        for (int i = strlen(b->current->text); i > x; i--)
        {
            buffer_remove_character(b, i, index);
        }
    }
    b->current->next = new_node;
    new_node->next = next_node;
}

void buffer_insert_character(Buffer *b, const char ch, const size_t x, const size_t index)
{
    if (b == NULL)
        return;

    buffer_set_current_node(b, index);
    if (b->current->text == NULL)
    {
        b->current->text = calloc(LINE_ALLOC_STEP, sizeof(char));
        b->current->allocs++;
    }

    if (strlen(b->current->text) + 2 > LINE_ALLOC_STEP * b->current->allocs)
    {
        char *temp_buffer = b->current->text;
        b->current->allocs++;
        b->current->text = calloc(LINE_ALLOC_STEP * b->current->allocs, sizeof(char));
        memcpy(b->current->text, temp_buffer, strlen(temp_buffer));
        free(temp_buffer);
        temp_buffer = NULL;
    }

    for (size_t i = 0; i < x; i++)
    {
        if (b->current->text[i] == 0)
        {
            b->current->text[i] = ' ';
        }
    }

    if (b->current->text[x] != 0)
    {
        for (size_t i = strlen(b->current->text); i > x; i--)
        {
            b->current->text[i + 1] = b->current->text[i];
        }
        // Required for when x = 0 since size_t is unsigned
        b->current->text[x + 1] = b->current->text[x];
    }

    b->current->text[x] = ch;
}

void buffer_remove_character(Buffer *b, const size_t x, const size_t index)
{
    if (b == NULL)
        return;

    buffer_set_current_node(b, index);
    if (b->current->text == NULL)
        return;

    for (int i = x - 1; i < strlen(b->current->text); i++)
    {
        b->current->text[i] = b->current->text[i + 1];
    }

    b->current->text[strlen(b->current->text)] = 0;
}

void buffer_remove_line(Buffer *b, const size_t index)
{
    node_t *removed_node;

    if (b == NULL)
        return;

    buffer_set_current_node(b, index);
    removed_node = b->current;

    if (removed_node != NULL && removed_node->text != NULL)
    {
        for (int i = 0; i < strlen(removed_node->text); i++)
        {
            buffer_insert_character(b, removed_node->text[i], buffer_get_text_len(b, index - 1), index - 1);
        }
    }

    free(removed_node->text);
    buffer_set_current_node(b, index - 1);
    b->current->next = removed_node->next;
    free(removed_node);
    removed_node = NULL;
}
