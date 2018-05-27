#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

#include "include/storage.h"
#include "include/consts.h"

#define PREFIX_LENGTH 6

static const char* note_prefix = "note_";

static int natural_sort( const void *a, const void *b )
{
    char a_buffer[PREFIX_LENGTH] = { 0 };
    char b_buffer[PREFIX_LENGTH] = { 0 };

    strncpy(a_buffer, *(const char**)a, strlen(note_prefix));
    strncpy(b_buffer, *(const char**)b, strlen(note_prefix));

    if( strcmp(a_buffer, note_prefix) == 0 && strcmp(b_buffer, note_prefix) == 0 ) {
        memset(a_buffer, 0, sizeof(a_buffer));
        memset(b_buffer, 0, sizeof(b_buffer));

        memcpy(a_buffer, *(const char**)a + strlen(note_prefix), strlen(*(const char**)a) - strlen(note_prefix));
        memcpy(b_buffer, *(const char**)b + strlen(note_prefix), strlen(*(const char**)b) - strlen(note_prefix));

        long a_id = strtol(a_buffer, NULL, 10 );
        long b_id = strtol(b_buffer, NULL, 10 );

        return a_id - b_id;
    }
    else {
        return strcasecmp(*(const char**)a, *(const char**)b);
    }
}

static int get_or_create_notes_directory( Storage *s, DIR **dir ) {
    if( s == NULL )
        return -1;

    if( s->home_directory == NULL ) {
        s->home_directory = getenv("HOME");
        if( s->home_directory == NULL ) 
            return -1;

        strcat(s->home_directory, "/.clunk");
    }

    // todo: fix memory leak of dir
    *dir = opendir( s->home_directory );
    if( *dir == NULL ) {
        if( mkdir( s->home_directory, 0700 ) == -1 ) 
            return -1;
        
        *dir = opendir( s->home_directory );
        if( *dir == NULL ) 
            return -1;
    }

    return 0;
}

void storage_cleanup( Storage *s ) {
    if( s == NULL )
        return;

    for( int i = 0; i < s->file_count; i++ ) {
        if( s->files[ i ] != NULL ) {
            free( s->files[ i ] );
            s->files[ i ] = NULL;
        }
    }

    if( s->files != NULL ) {
        free( s->files );
        s->files = NULL;
    }

    s->file_count = 0;
    s->allocs = 0;
}

int storage_get_notes( Storage *s ) {
    DIR *dir = NULL;
    struct dirent *dp;   

    if( s == NULL ) 
        return -1;

    if( get_or_create_notes_directory(s, &dir) == -1 ) 
        return -1;

    s->files = calloc( NOTES_ALLOC_STEP, sizeof(char*) );
    s->allocs++;

    while ((dp = readdir (dir)) != NULL) {
        if( dp->d_name[0] == '.' )
            continue;

        s->files[s->file_count] = calloc(1, strlen(dp->d_name) + 1);
        strcpy(s->files[s->file_count], dp->d_name);

        s->file_count++;
        if( s->file_count + 1 > NOTES_ALLOC_STEP * s->allocs ) {
            char **temp_note_buffer = s->files;
            s->allocs++;
            s->files = calloc( s->allocs * NOTES_ALLOC_STEP, sizeof(char*));
            memcpy(s->files, temp_note_buffer, (s->file_count) * sizeof(char*));
            free(temp_note_buffer);
            temp_note_buffer = NULL;
        }
    }

    qsort( s->files, s->file_count, sizeof (char *), natural_sort );

    return 0;
}

int storage_create_note( const Storage *s, char *created_name ) {
    char *note_name = NULL;
    FILE *tf = NULL;
    int i = 0;

    if( s == NULL )
        return -1;

    int digit_count = 0;
    int file_count = s->file_count;
    while( (file_count /= 10) > 0 ) {
        digit_count++;
    }

    note_name = calloc(strlen(s->home_directory) + strlen("/") + strlen(note_prefix) + digit_count + 1, sizeof(char));

    for( i = 0; i < s->file_count + 1; i++ ) {
        sprintf(note_name, "%s/%s%d", s->home_directory, note_prefix, i + 1);
        if( ( tf = fopen(note_name, "r") ) ){
            fclose( tf );
        }
        else {
            tf = fopen( note_name, "w" );
            fclose( tf );
            break;
        }
    }

    if( created_name != NULL ) {
        sprintf(created_name, "%s%d", note_prefix, i + 1 );
    }
 
    if( note_name != NULL ) {
        free( note_name );
        note_name = NULL;
    }

    return 0;
}

int storage_delete_note( const Storage *s, const int file_index ) {
    char *note_name = NULL;

    if( s == NULL )
        return -1;

    note_name = calloc(strlen(s->home_directory) + 1 + strlen(s->files[file_index]) + 1, sizeof(char));

    sprintf(note_name, "%s/%s", s->home_directory, s->files[file_index]);
    remove( note_name );

    if( note_name != NULL ) {
        free( note_name );
        note_name = NULL;
    }

    return 0;
}

void storage_save_note( const Storage *s, const int file_index, const char *text ) {
    char *note_name = NULL;
    FILE *note_stream = NULL;

    if( s == NULL || text == NULL )
        return;

    note_name = calloc(strlen(s->home_directory) + 1 + strlen(s->files[file_index]) + 1, sizeof(char));

    sprintf(note_name, "%s/%s", s->home_directory, s->files[file_index]);

    note_stream = fopen( note_name, "w" );
    fputs( text, note_stream );
    fclose( note_stream );

    if( note_name != NULL ) {
        free( note_name );
        note_name = NULL;
    }
}

void storage_rename_note( const Storage *s, const int file_index, const char *name) {
    FILE *tf = NULL;
    char *old_note_name = NULL;
    char *new_note_name = NULL;

    if( s == NULL || name == NULL )
        return;

    old_note_name = calloc(strlen(s->home_directory) + 1 + strlen(s->files[file_index]) + 1, sizeof(char));
    sprintf(old_note_name, "%s/%s", s->home_directory, s->files[file_index]);
    
    new_note_name = calloc(strlen(s->home_directory) + 1 + strlen(name) + 1, sizeof(char));
    sprintf(new_note_name, "%s/%s", s->home_directory, name);

    if( !( tf = fopen(new_note_name, "r") ) ){
        rename( old_note_name, new_note_name );
    }
    
    if( tf != NULL )
        fclose( tf );

    free( old_note_name );
    free( new_note_name );
}

int storage_find_note_index(const Storage *s, const char *note_name) {
    int index = 0;
    
    if( note_name == NULL )
        return -1;

    for( int i = 0; i < s->file_count; i++ ) {
        if( strcmp( s->files[ i ], note_name ) == 0 ) {
            return i;
        }
    }

    return index;
}