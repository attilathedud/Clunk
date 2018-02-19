#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define KEY_CTRL_X 24

char *files[3];

void print_file( int index ) {
    int c = 0;
    int col = 0;

    FILE *fptr = NULL;

    fptr = fopen(files[index], "r");
    if( fptr == NULL ) {
        return;
    }

    while((c = fgetc(fptr)) != EOF) {
        mvprintw( 4, col, &c );
        col++;
    }

    fclose(fptr);
}

int main( int argc, char** argv ) {
    int ch = 0;

    int cur_file = 0;
    
    initscr( );
    noecho( );
    cbreak( );
    keypad( stdscr, TRUE );

    DIR *dir;
    struct dirent *dp;

    dir = opendir(".");

    int file_index = 0;

    while ((dp = readdir (dir)) != NULL) {
        if( strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 || strcmp(dp->d_name, "notes") == 0 )
            continue;

        files[file_index] = calloc(1, strlen(dp->d_name) + 1);
        strcpy(files[file_index], dp->d_name);

        file_index++;
    }

    closedir(dir);

    for( int i = 0; i < 3; i++ ) {
        mvprintw( i, 1, files[ i ] );
    }

    mvprintw(0, 0, ">");

    print_file( cur_file );

    while( ( ch = getch( ) ) != KEY_CTRL_X ) 
    {
        int old_cur_file = cur_file;

        switch( ch ) {
            case KEY_DOWN:
                cur_file++;
                if( cur_file > 2 ) {
                    cur_file = 0;
                }
                break;
            case KEY_UP:
                cur_file--;
                if( cur_file < 0 ) {
                    cur_file = 2;
                }
                break;
        }

        if( old_cur_file != cur_file ) {
            print_file( cur_file );
            mvprintw(old_cur_file, 0, " ");
            mvprintw(cur_file, 0, ">");
        }
    }

    refresh( );
    endwin( );

    for( int i = 0; i < 3; i++ ) {
        if( files[ i ] != NULL ) {
            free( files[ i ]);
        }
    }

    return 0;
}