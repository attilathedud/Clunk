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

    s->file_count = 0;
}

int get_notes_in_directory( Storage *s ) {
    DIR *dir = NULL;
    struct dirent *dp;   

    if( s == NULL ) 
        return -1;

    if( get_or_create_notes_directory(s, &dir) == -1 ) 
        return -1;
    
    while ((dp = readdir (dir)) != NULL) {
        if( strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 || strcmp(dp->d_name, ".DS_Store") == 0 )
            continue;

        s->files[s->file_count] = calloc(1, strlen(dp->d_name) + 1);
        strcpy(s->files[s->file_count], dp->d_name);

        s->file_count++;
    }

    return 0;
}

int create_note( Storage *s ) {
    char note_name[256] = { 0 };    //todo fix path length issue
    FILE *temp_file;

    if( s == NULL )
        return -1;

    for( int i = 0; i < MAX_NOTES; i++ ) {
        sprintf(note_name, "%s/note_%d", s->home_directory, i + 1);
        if( ( temp_file = fopen(note_name, "r") ) ){
            fclose( temp_file );
        }
        else {
            temp_file = fopen( note_name, "w" );
            fclose( temp_file );
            break;
        }
    }

    return 0;
}

int delete_note( Storage *s, int file_index ) {
    if( s == NULL )
        return -1;

    char note_name[256] = { 0 };
    sprintf(note_name, "%s/%s", s->home_directory, s->files[file_index]);
    remove( note_name );

    return 0;
}