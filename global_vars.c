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

#include "global_vars.h"
#include "callbacks.h"

cairo_surface_t* kpixmap;
GtkWidget *karea;
GtkWidget *clear_button;
GtkWidget *undo_button;
GtkWidget *lookup_button;
GtkWidget *jukugo_entry;
GtkWidget *results_display_box;
GtkWidget *scrollingResults;
GtkWidget *jishoLink;
GtkUIManager *ui_manager;

gulong jukugo_entry_insert_text_handler_id;

kp_wchar kanjiguess[MAX_GUESSES];
int num_guesses = 0;
kp_wchar kselected;

PadArea *pad_area;
Keyboard keyboard = {KBD_INIT_VISIBLE, FALSE, NULL};

/* globals for engine communication */
int engine_pid;
GIOChannel *from_engine;
GIOChannel *to_engine;

char *data_file = NULL;
const char *progname;

int WINDOW_WIDTH = DEFAULT_WINDOW_WIDTH;
int WINDOW_HEIGHT = DEFAULT_WINDOW_HEIGHT;
int WINDOW_RESIZABLE = 1;

gboolean hiragana_replace = HIRAGANA_INIT_REPLACE;

GtkActionEntry entries[] = {

    { "FileMenuAction", NULL, "_File" },                  /* name, stock id, label */
    { "CharacterMenuAction", NULL, "_Character" },
    { "EditMenuAction", NULL, "_Edit" },

    //file  
    { "QuitAction", NULL,
      "_Quit", "<control>Q",    
      "Quit",
      G_CALLBACK (exit_callback) },

    //edit
    { "UndoAction", NULL,
      "_Undo", "<control>Z",  
      "undo",
      G_CALLBACK (undo_callback) },

    { "CopyAction", NULL,
      "_Copy", "<control>C",  
      "copy",
      G_CALLBACK (copy_callback) },

    //character
    { "LookupAction", NULL,   
      "_Lookup", "<control>L",          
      "Look-up the character drawn",    
      G_CALLBACK (look_up_callback) },
    
    { "ClearAction", NULL,
      "_Clear","<control>X",  
      "Clear the drawing area",
      G_CALLBACK (clear_callback) },

    { "SaveAction", NULL,
      "_Save","<control>S",  
      "write out the points in the character, \
and the selected character to a file \"samples.dat\" in the \
current directory. This is intended for making a file of \
characters for automated testing",
      G_CALLBACK (save_callback) }
};

guint n_entries = G_N_ELEMENTS (entries);

GtkToggleActionEntry toggle_entries[] = {
    { "AnnotateAction", NULL,
      "_Annotate", NULL,  
      "Display stroke order",
      G_CALLBACK (annotate_callback), TRUE }, 

    { "AutoLookupAction", NULL,
      "_Auto Lookup", NULL,  
      "Lookup when a stroke is drawn",
      G_CALLBACK (auto_look_up_callback), TRUE },

    { "KeyboardAction", NULL,
      "_Keyboard", NULL,
      "Toggle keyboard visibility",
      G_CALLBACK (toggle_keyboard_callback), KBD_INIT_VISIBLE },
    
    { "ReplaceHiraganaAction", NULL,
      "_Replace Hiragana", NULL,
      "Replace roman letters with hiragana",
      G_CALLBACK (toggle_hiragana_replace_callback), HIRAGANA_INIT_REPLACE }
};

guint n_toggle_entries = G_N_ELEMENTS (toggle_entries);
