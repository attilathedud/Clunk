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

static const char* note_prefix = "note_";

static int natural_sort( const void *a, const void *b )
{
    char a_buffer[6] = { 0 };
    char b_buffer[6] = { 0 };

    strncpy(a_buffer, *(const char**)a, strlen(note_prefix));
    strncpy(b_buffer, *(const char**)b, strlen(note_prefix));

    if( strcmp(a_buffer, note_prefix) == 0 && strcmp(b_buffer, note_prefix) == 0 ) {
        memset(a_buffer, 0, 6);
        memset(b_buffer, 0, 6);

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

        strcat(s->home_directory, "/.terminalnotes");
    }

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
        }
    }

    if( s->files != NULL ) {
        free( s->files );
    }

    s->file_count = 0;
}

int get_notes_in_directory( Storage *s ) {
    DIR *dir = NULL;
    struct dirent *dp;   

    if( s == NULL ) 
        return -1;

    if( get_or_create_notes_directory(s, &dir) == -1 ) 
        return -1;

    s->files = calloc( MAX_NOTES, sizeof(char*) );

    while ((dp = readdir (dir)) != NULL) {
        if( strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 || strcmp(dp->d_name, ".DS_Store") == 0 )
            continue;

        s->files[s->file_count] = calloc(1, strlen(dp->d_name) + 1);
        strcpy(s->files[s->file_count], dp->d_name);

        s->file_count++;
        if( s->file_count > MAX_NOTES - 1 ) {
            char **temp_note_buffer = s->files;
            s->files = calloc( s->file_count + MAX_NOTES, sizeof(char*));
            memcpy(s->files, temp_note_buffer, (s->file_count) * sizeof(char*));
            free(temp_note_buffer);
        }
    }

    qsort( s->files, s->file_count, sizeof (char *), natural_sort );

    return 0;
}

int create_note( const Storage *s ) {
    char *note_name;
    FILE *temp_file;

    if( s == NULL )
        return -1;

    //todo fix digit allocation
    note_name = calloc(1, strlen(s->home_directory) + 1 + strlen(note_prefix) + 2 + 1);

    for( int i = 0; i < s->file_count + 1; i++ ) {
        sprintf(note_name, "%s/%s%d", s->home_directory, note_prefix, i + 1);
        if( ( temp_file = fopen(note_name, "r") ) ){
            fclose( temp_file );
        }
        else {
            temp_file = fopen( note_name, "w" );
            fclose( temp_file );
            break;
        }
    }

    free( note_name );

    return 0;
}

int delete_note( const Storage *s, const int file_index ) {
    char *note_name;

    if( s == NULL )
        return -1;

    note_name = calloc(1, strlen(s->home_directory) + 1 + strlen(s->files[file_index]) + 1);

    sprintf(note_name, "%s/%s", s->home_directory, s->files[file_index]);
    remove( note_name );

    free( note_name );

    return 0;
}