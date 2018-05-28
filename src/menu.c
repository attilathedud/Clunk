#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "include/consts.h"
#include "include/storage.h"
#include "include/menu.h"

#define ABOUT_TEXT_LINES 16

#define ADD_CHAR_TO_RENAME_BUFFER                            \
    if (m->is_renaming_file)                                 \
    {                                                        \
        if (strlen(m->rename_buffer) < RENAME_BUFFER_LEN)    \
        {                                                    \
            m->rename_buffer[strlen(m->rename_buffer)] = ch; \
        }                                                    \
    }

#define SAVE_NOTE                   \
    note_text = editor_get_text(e); \
    menu_save_note(m, note_text);   \
    free(note_text);

#define STORAGE_RESET         \
    storage_cleanup(&(m->s)); \
    storage_get_notes(&(m->s));

static char about_text[ABOUT_TEXT_LINES][100] = {
    "##### #     #   # #   # #  #\0",
    "#     #     #   # # # # # # \0",
    "#     #     #   # #  ## #   \0",
    "#     #     #   # #   # # # \0",
    "##### ##### ##### #   # #  #\0",
    "Just take notes.\0",
    "\0",
    "Clunk focuses on taking notes and getting out of the way.\0",
    "\0",
    "- Notes are stored locally in ~/.clunk/ \0",
    "- All prompts will be in the lower left \0",
    "- Notes are NOT automatically saved     \0",
    "- Lines beginning with ^ are highlighted\0",
    "\0",
    "Code is available at github.com/attilathedud/Clunk\0"};

int menu_init(Menu *m)
{
    if (m == NULL)
        return -1;

    if (storage_get_notes(&(m->s)) == -1)
    {
        return -1;
    }

    if (m->s.file_count == 0)
    {
        storage_create_note(&(m->s), NULL);
        storage_cleanup(&(m->s));
        storage_get_notes(&(m->s));
    }

    m->has_changed_file = true;
    m->display_about_screen = false;

    return 0;
}

int menu_handle_input(Menu *m, Editor *e, const int ch)
{
    int pass_input_to_editor = false;
    char *note_text = NULL;

    // The rename buffer enforces a limit of 128 characters before adding to the buffer
    char created_note_name[128] = {0};

    if (m == NULL)
        return -1;

    m->has_changed_file = false;

    if (ch == KEY_F(9))
    {
        m->is_deleting_file = false;
        m->is_renaming_file = false;
        m->has_changed_file = false;
        pass_input_to_editor = false;
        m->display_about_screen = !m->display_about_screen;
        return pass_input_to_editor;
    }

    if (m->display_about_screen)
    {
        m->display_about_screen = false;
        return false;
    }

    switch (ch)
    {
    case KEY_F(1):
        if (m->s.file_count > 0)
        {
            m->selected_file_index--;
            if (m->selected_file_index < 0)
            {
                m->selected_file_index = m->s.file_count - 1;
                m->scroll_offset = m->s.file_count + 1 - LINES / 2;
                if (m->scroll_offset < 0)
                {
                    m->scroll_offset = 0;
                }
            }
            if (m->selected_file_index - m->scroll_offset < 0)
            {
                m->scroll_offset--;
            }
            m->has_changed_file = true;
        }
        break;
    case KEY_F(2):
        if (m->s.file_count > 0)
        {
            m->selected_file_index++;
            if (m->selected_file_index > m->s.file_count - 1)
            {
                m->selected_file_index = 0;
                m->scroll_offset = 0;
            }
            if ((m->selected_file_index - m->scroll_offset + 1) * 2 > LINES - 1)
            {
                m->scroll_offset++;
            }
            m->has_changed_file = true;
        }
        break;
    case KEY_F(5):
        if (m->s.file_count > 0)
        {
            SAVE_NOTE
            attron(COLOR_PAIR(1));
            mvprintw(LINES - 2, 0, "Note saved");
            attroff(COLOR_PAIR(1));
        }
        break;
    case KEY_F(6):
        memset(created_note_name, 0, sizeof(created_note_name));
        storage_create_note(&(m->s), created_note_name);
        STORAGE_RESET
        m->selected_file_index = storage_find_note_index(&(m->s), created_note_name);
        m->is_renaming_file = true;
        m->has_changed_file = true;
        memset(m->rename_buffer, 0, sizeof(m->rename_buffer));
        break;
    case KEY_F(7):
        if (m->is_renaming_file)
        {
            m->is_renaming_file = false;
            break;
        }

        if (m->s.file_count > 0)
        {
            m->is_renaming_file = true;
            m->is_deleting_file = false;
            memset(m->rename_buffer, 0, sizeof(m->rename_buffer));
        }
        break;
    case KEY_F(8):
        if (m->is_deleting_file)
        {
            m->is_deleting_file = false;
            break;
        }

        if (m->s.file_count > 0)
        {
            m->is_deleting_file = true;
            m->is_renaming_file = false;
        }
        break;
    case KEY_LOWER_Y:
    case KEY_UPPER_Y:
        if (!m->is_deleting_file && !m->is_renaming_file)
        {
            pass_input_to_editor = true;
            break;
        }

        ADD_CHAR_TO_RENAME_BUFFER
        if (m->is_renaming_file)
            break;

        if (m->selected_file_index < 0 || m->selected_file_index > m->s.file_count - 1)
            break;

        storage_delete_note(&(m->s), m->selected_file_index);
        STORAGE_RESET

        if (m->selected_file_index > m->s.file_count - 1)
        {
            m->selected_file_index = (m->s.file_count - 1 < 0) ? 0 : m->s.file_count - 1;
        }

        if ((m->selected_file_index - m->scroll_offset + 1) * 2 < LINES - 2)
        {
            m->scroll_offset--;
        }

        if (m->scroll_offset < 0)
        {
            m->scroll_offset = 0;
        }

        m->has_changed_file = true;
        m->is_deleting_file = false;
        break;
    case KEY_LOWER_N:
    case KEY_UPPER_N:
        if (!m->is_deleting_file && !m->is_renaming_file)
        {
            pass_input_to_editor = true;
        }
        m->is_deleting_file = false;
        m->has_changed_file = false;

        ADD_CHAR_TO_RENAME_BUFFER
        break;
    case KEY_RETURN:
        if (!m->is_renaming_file)
        {
            pass_input_to_editor = true;
            break;
        }

        SAVE_NOTE
        storage_rename_note(&(m->s), m->selected_file_index, m->rename_buffer);
        STORAGE_RESET
        m->scroll_offset = 0;
        m->selected_file_index = storage_find_note_index(&(m->s), m->rename_buffer);
        while ((m->selected_file_index - m->scroll_offset + 1) * 2 > LINES - 1)
        {
            m->scroll_offset++;
        }

        m->is_renaming_file = false;
        break;
    // For some reason, ncurses traps signals to esc and won't pass them immediately to a user program
    case KEY_ESC:
        m->is_deleting_file = false;
        m->is_renaming_file = false;
        m->has_changed_file = false;
        break;
    case KEY_DELETE:
        if (!m->is_renaming_file)
        {
            pass_input_to_editor = true;
            break;
        }

        if (strlen(m->rename_buffer) > 0)
        {
            m->rename_buffer[strlen(m->rename_buffer) - 1] = 0;
        }

        break;
    default:
        if (!m->is_renaming_file)
        {
            pass_input_to_editor = true;
        }
        else
            ADD_CHAR_TO_RENAME_BUFFER
        break;
    }

    return pass_input_to_editor;
}

void menu_save_note(Menu *m, const char *text)
{
    if (m == NULL || text == NULL)
        return;

    storage_save_note(&(m->s), m->selected_file_index, text);
}

void menu_print(Menu *m)
{
    if (m->display_about_screen)
    {
        for (int i = 0; i < ABOUT_TEXT_LINES; i++)
        {
            int screen_text_middle = ((COLS - NOTES_OFFSET) / 2) - (strlen(about_text[i]) / 2) + NOTES_OFFSET;
            mvprintw(i, screen_text_middle, about_text[i]);
        }
    }
    else
    {
        for (int i = 0; i + m->scroll_offset < m->s.file_count; i++)
        {
            if (i + m->scroll_offset == m->selected_file_index)
            {
                attron(COLOR_PAIR(1));
                move((i * 2) + 1, 5);
                printw(" ");
                printw(m->s.files[i + m->scroll_offset]);
                printw(" ");
                attroff(COLOR_PAIR(1));
            }
            else
            {
                mvprintw((i * 2) + 1, 1, m->s.files[i + m->scroll_offset]);
            }
        }

        if (m->is_deleting_file)
        {
            attron(COLOR_PAIR(1));
            mvprintw(LINES - 2, 0, "Are you sure? (y/n):");
            attroff(COLOR_PAIR(1));
        }
        else if (m->is_renaming_file)
        {
            attron(COLOR_PAIR(1));
            mvprintw(LINES - 2, 0, "Name (enter): ");
            printw(m->rename_buffer);
            attroff(COLOR_PAIR(1));
        }
    }
}

void menu_cleanup(Menu *m)
{
    storage_cleanup(&(m->s));
}
