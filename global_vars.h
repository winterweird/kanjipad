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

#ifndef KANJIPAD_GLOBAL_VARIABLES_HEADER
#define KANJIPAD_GLOBAL_VARIABLES_HEADER

#include "preprocessor_definitions.h"
#include "datastructure_definitions.h"

/* user interface elements */
//static GdkPixmap *kpixmap;
extern cairo_surface_t* kpixmap;
extern GtkWidget* karea;
extern GtkWidget* clear_button;
extern GtkWidget* undo_button;
extern GtkWidget* lookup_button;
extern GtkWidget* jukugo_entry;
extern GtkWidget* results_display_box;
extern GtkWidget* scrollingResults;
extern GtkWidget* jishoLink;
extern GtkUIManager* ui_manager;

extern kp_wchar kanjiguess[MAX_GUESSES];
extern int num_guesses;
extern kp_wchar kselected;

extern PadArea* pad_area;
extern Keyboard keyboard;

/* globals for engine communication */
extern int engine_pid;
extern GIOChannel* from_engine;
extern GIOChannel* to_engine;

extern char* data_file;
extern const char* progname;

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern int WINDOW_RESIZABLE;

// 9 is hardcoded based on the count I made earlier, this might create problems.
extern GtkActionEntry entries[9];

extern guint n_entries;

// 3 is similarly hardcoded
// NOTE: was 2 until I added the option to toggle a virtual keyboard
extern GtkToggleActionEntry toggle_entries[3];

extern guint n_toggle_entries;

#endif /* KANJIPAD_GLOBAL_VARIABLES_HEADER */
