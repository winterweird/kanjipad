/* KanjiPad - Japanese handwriting recognition front end
 * Copyright (C) 1997 Owen Taylor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/** Refactored and modified by Vegard Itland (2017) **/
#ifndef KANJIPAD_CALLBACK_FUNCTIONS_HEADER
#define KANJIPAD_CALLBACK_FUNCTIONS_HEADER

#include <gtk/gtkcheckmenuitem.h>

void exit_callback();
void undo_callback();
void copy_callback();
void save_callback();
void clear_callback();
void look_up_callback();
void append_jukugo_callback();
void query_jisho();
void clear_search_field();
void insert_char_callback();
void delete_char_callback();
void cursor_pos_left_callback();
void cursor_pos_right_callback();
void replace_hiragana_callback();
void show_keyboard_callback();
void hide_keyboard_callback();
void annotate_callback(GtkCheckMenuItem *menu_item, gpointer user_data);
void auto_look_up_callback(GtkCheckMenuItem *menu_item, gpointer user_data);
void toggle_keyboard_callback(GtkCheckMenuItem *menu_item, gpointer user_data);
void toggle_hiragana_replace_callback(GtkCheckMenuItem *menu_item, gpointer user_data);

// function which handles keypresses (bound to window)
gboolean handle_keypress_callback(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

// function which handles keypresses (bound to jukugo_entry)
gboolean jukugo_keypress_callback(GtkWidget* widget, GdkEventKey* event, gpointer user_data);

#endif /* KANJIPAD_CALLBACK_FUNCTIONS_HEADER */
