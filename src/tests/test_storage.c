#include <stdio.h>
#include <criterion/criterion.h>

#include "include/storage.h"

Storage s = {0};

void storage_setup()
{
    memset(&s, 0, sizeof(Storage));
}

void storage_teardown()
{
    storage_cleanup(&s);
}

Test(storage, cleanup, .init = storage_setup)
{
    s.file_count++;
    storage_cleanup(&s);
    cr_assert(s.file_count == 0);
}

Test(storage, get_notes, .init = storage_setup, .fini = storage_teardown)
{
    char *home_directory = NULL;

    char *user_env = getenv("HOME");

    home_directory = calloc(strlen(user_env) + strlen("/.clunk/test_note") + 1, sizeof(char));
    strcpy(home_directory, user_env);
    strcat(home_directory, "/.clunk");

    storage_get_notes(&s, NULL);

    cr_assert(strcmp(home_directory, s.home_directory) == 0);
}

Test(storage, create_note, .init = storage_setup, .fini = storage_teardown)
{
    const char *note_prefix = "note_";
    char *note_name = NULL;
    char created_note_name[128] = {0};

    FILE *tf = NULL;

    storage_get_notes(&s, NULL);

    int digit_count = 0;
    int file_count = s.file_count;
    while ((file_count /= 10) > 0)
    {
        digit_count++;
    }

    note_name = calloc(strlen(s.home_directory) + strlen("/") + strlen(note_prefix) + digit_count + 1, sizeof(char));

    storage_create_note(&s, created_note_name);

    sprintf(note_name, "%s/%s", s.home_directory, created_note_name);

    if ((tf = fopen(note_name, "r")))
    {
        fclose(tf);
        cr_assert(1);
    }
    else
    {
        cr_assert(0);
    }

    remove(note_name);
    free(note_name);
}

Test(storage, delete_note, .init = storage_setup, .fini = storage_teardown)
{
    FILE *tf = NULL;
    char *note_path = NULL;

    storage_get_notes(&s, NULL);

    char *user_env = getenv("HOME");

    note_path = calloc(strlen(user_env) + strlen("/.clunk/test_delete_note") + 1, sizeof(char));
    strcpy(note_path, user_env);
    strcat(note_path, "/.clunk/test_delete_note");

    if ((tf = fopen(note_path, "w")))
    {
        fclose(tf);
    }

    storage_cleanup(&s);
    storage_get_notes(&s, NULL);

    storage_delete_note(&s, storage_find_note_index(&s, "test_delete_note"));

    if ((tf = fopen(note_path, "r")))
    {
        fclose(tf);
        cr_assert(0);
    }
    else
    {
        cr_assert(1);
    }

    free(note_path);
}

Test(storage, save_note, .init = storage_setup, .fini = storage_teardown)
{
    FILE *tf = NULL;
    char *note_path = NULL;

    char *line = NULL;
    size_t n = 0;

    storage_get_notes(&s, NULL);

    char *user_env = getenv("HOME");

    note_path = calloc(strlen(user_env) + strlen("/.clunk/test_save_note") + 1, sizeof(char));
    strcpy(note_path, user_env);
    strcat(note_path, "/.clunk/test_save_note");

    if ((tf = fopen(note_path, "w")))
    {
        fclose(tf);
    }

    storage_cleanup(&s);
    storage_get_notes(&s, NULL);
    storage_save_note(&s, storage_find_note_index(&s, "test_save_note"), "test");

    if ((tf = fopen(note_path, "r")))
    {
        getline(&line, &n, tf);
        cr_assert(strcmp(line, "test") == 0);
        fclose(tf);
    }

    remove(note_path);
    free(note_path);
}

Test(storage, rename_note, .init = storage_setup, .fini = storage_teardown)
{
    FILE *tf = NULL;
    char *note_path = NULL;

    storage_get_notes(&s, NULL);

    char *user_env = getenv("HOME");

    note_path = calloc(strlen(user_env) + strlen("/.clunk/test_rename_note") + 1, sizeof(char));
    strcpy(note_path, user_env);
    strcat(note_path, "/.clunk/test_rename_note");

    if ((tf = fopen(note_path, "w")))
    {
        fclose(tf);
    }

    storage_cleanup(&s);
    storage_get_notes(&s, NULL);
    storage_rename_note(&s, storage_find_note_index(&s, "test_rename_note"), "test_newname_note");

    if ((tf = fopen(note_path, "r")))
    {
        fclose(tf);
        cr_assert(0);
        remove(note_path);
    }

    strcpy(note_path, user_env);
    strcat(note_path, "/.clunk/test_newname_note");
    if ((tf = fopen(note_path, "r")))
    {
        fclose(tf);
        cr_assert(1);
        remove(note_path);
    }

    free(note_path);
}
