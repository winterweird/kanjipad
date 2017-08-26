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

/** Refactored by Vegard Itland, 2017-08-26 **/

// This header declares apparently karea-related functions.

#ifndef KAREA_RELATED_FUNCTIONS_HEADER
#define KAREA_RELATED_FUNCTIONS_HEADER

#include "datastructure_definitions.h"

void karea_get_char_size(GtkWidget* widget, int* width, int* height);

gchar* utf8_for_char(kp_wchar ch);

void karea_draw_character(GtkWidget* w, int index, int selected);

void karea_draw(GtkWidget *w);

int karea_configure_event(GtkWidget *w, GdkEventConfigure *event);

int karea_expose_event(GtkWidget *w, GdkEventExpose *event);

int karea_erase_selection(GtkWidget* w);

void karea_primary_clear(GtkClipboard* clipboard, gpointer owner);

void karea_primary_get(GtkClipboard* clipboard, GtkSelectionData* selection_data,
                       guint info, gpointer owner);

int karea_button_press_event(GtkWidget* w, GdkEventButton* event);

#endif /* KAREA_RELATED_FUNCTIONS_HEADER */
