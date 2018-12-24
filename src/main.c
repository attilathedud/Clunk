#include <ncurses.h>

#include "include/consts.h"
#include "include/menu.h"
#include "include/editor.h"

static void print_help_line()
{
    move(LINES - 1, 0);
    attron(COLOR_PAIR(1));
    printw("F1");
    attroff(COLOR_PAIR(1));
    printw("Prev ");
    attron(COLOR_PAIR(1));
    printw("F2");
    attroff(COLOR_PAIR(1));
    printw("Next ");

    move(LINES - 1, COLS / 3);
    attron(COLOR_PAIR(1));
    printw("F5");
    attroff(COLOR_PAIR(1));
    printw("Save ");
    attron(COLOR_PAIR(1));
    printw("F6");
    attroff(COLOR_PAIR(1));
    printw("Create ");
    attron(COLOR_PAIR(1));
    printw("F7");
    attroff(COLOR_PAIR(1));
    printw("Rename ");
    attron(COLOR_PAIR(1));
    printw("F8");
    attroff(COLOR_PAIR(1));
    printw("Delete ");

    move(LINES - 1, COLS - 15);
    attron(COLOR_PAIR(1));
    printw("F9");
    attroff(COLOR_PAIR(1));
    printw("About ");
    attron(COLOR_PAIR(1));
    printw("F10");
    attroff(COLOR_PAIR(1));
    printw("Exit");
}

int main(int argc, char **argv)
{
    int ch = 0;

    Menu m = {{0}};
    Editor e = {{0}};

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    use_default_colors();

    if (menu_init(&m, argv[1]) == -1)
    {
        refresh();
        endwin();
        fprintf(stderr, "Error getting the notes directory.\n");
        return -1;
    }

    editor_load_file(&e, m.s.home_directory, m.s.files[0]);

    do
    {
        erase();

        // If the menu is controlled, don't pass input to editor
        if (menu_handle_input(&m, &e, ch))
        {
            editor_handle_input(&e, ch);
        }

        if (m.has_changed_file)
        {
            editor_load_file(&e, m.s.home_directory, m.s.files[m.selected_file_index]);
        }

        menu_print(&m);

        if (!m.display_about_screen)
            editor_print(&e);

        // Print separator line
        mvvline(0, MENU_OFFSET, ACS_VLINE, LINES - 1 - m.is_renaming_file);
        print_help_line();

        editor_print_cursor(&e);
    } while ((ch = getch()) != KEY_F(10));

    menu_cleanup(&m);
    editor_cleanup(&e);

    refresh();
    endwin();

    return 0;
}
