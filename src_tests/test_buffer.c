#include <stdio.h>
#include <criterion/criterion.h>

#include "include/buffer.h"

Buffer b = {0};

void buffer_setup()
{
    memset(&b, 0, sizeof(Buffer));
}

void buffer_teardown()
{
    buffer_cleanup(&b);
}

Test(buffer, append_line, .init = buffer_setup, .fini = buffer_teardown)
{
    buffer_append_line(&b, "test", strlen("test"));
    cr_assert(b.head != NULL);
    cr_assert(strcmp(b.head->text, "test") == 0);
}

Test(buffer, append_line_multiple, .init = buffer_setup, .fini = buffer_teardown)
{
    buffer_append_line(&b, "test", strlen("test"));
    buffer_append_line(&b, "test", strlen("test2"));
    cr_assert(b.head != NULL);
    cr_assert(b.head->next != NULL);
    cr_assert(strcmp(b.head->text, "test") == 0);
    cr_assert(strcmp(b.head->next->text, "test") == 0);
}

Test(buffer, get_text_length, .init = buffer_setup, .fini = buffer_teardown)
{
    buffer_append_line(&b, "test", strlen("test"));
    cr_assert(buffer_get_text_len(&b, 0) == strlen("test"));
}

Test(buffer, insert_character, .init = buffer_setup, .fini = buffer_teardown)
{
    buffer_insert_character(&b, 't', 0, 0);
    buffer_insert_character(&b, 'e', 2, 0);
    cr_assert(strcmp(b.head->text, "t e") == 0);
}

Test(buffer, remove_character, .init = buffer_setup, .fini = buffer_teardown)
{
    buffer_insert_character(&b, 't', 0, 0);
    buffer_insert_character(&b, 'e', 1, 0);
    buffer_remove_character(&b, 1, 0);
    cr_assert(strlen(b.head->text) == 1);
}

Test(buffer, remove_line, .init = buffer_setup, .fini = buffer_teardown)
{
    buffer_insert_character(&b, 't', 0, 0);
    buffer_insert_character(&b, 'e', 0, 1);
    buffer_insert_character(&b, 's', 0, 2);
    buffer_remove_line(&b, 1);
    cr_assert(b.head->next->text[0] == 's');
}

Test(buffer, split_line, .init = buffer_setup, .fini = buffer_teardown)
{
    buffer_insert_character(&b, 't', 0, 0);
    buffer_insert_character(&b, 'e', 1, 0);
    buffer_split_line(&b, 1, 0);
    cr_assert(b.head->next->text[0] == 'e');
}