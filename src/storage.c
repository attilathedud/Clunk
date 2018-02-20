#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

#include "include/storage.h"

static int get_or_create_notes_directory( DIR **dir ) {
    char *home_directory = getenv("HOME");
    if( home_directory == NULL ) 
        return -1;
    

    strcat(home_directory, "/.terminalnotes");

    *dir = opendir( home_directory );
    if( *dir == NULL ) {
        if( mkdir( home_directory, 0700 ) == -1 ) 
            return -1;
        
        *dir = opendir( home_directory );
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
}

int get_notes_in_directory( Storage *s ) {
    DIR *dir = NULL;
    struct dirent *dp;   

    if( s == NULL ) 
        return -1;

    if( get_or_create_notes_directory(&dir) == -1 ) 
        return -1;
    

    while ((dp = readdir (dir)) != NULL) {
        if( strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 )
            continue;

        s->files[s->file_count] = calloc(1, strlen(dp->d_name) + 1);
        strcpy(s->files[s->file_count], dp->d_name);

        s->file_count++;
    }

    return 0;
}