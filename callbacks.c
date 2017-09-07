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

#include "callbacks.h"
#include "global_vars.h"
#include <errno.h> // errno
#include <stdlib.h> // exit
#include <string.h> // exit
#include <gdk/gdkkeysyms.h>  // event codes
#include "karea.h" // utf8_for_char
#include "kanjipad.h" // pad_area functions
#include "sensitivity.h" // update_sensitivity
#include "jisho_search.h" // jisho_search_keyword
#include "json_to_gtk.h" // display_results

void exit_callback(GtkWidget* w) {
    exit(0);
}

void undo_callback(GtkWidget* w) {
    pad_area_undo_stroke (pad_area);
}

void copy_callback(GtkWidget* w) {
    if (kselected.d[0] || kselected.d[1]) {
        gchar *string_utf = utf8_for_char (kselected);
        gtk_clipboard_set_text (gtk_clipboard_get (GDK_SELECTION_CLIPBOARD), string_utf, -1);
        g_free (string_utf);
    }
}

void save_callback(GtkWidget* w) {
    static int unknownID = 0;
    static FILE *samples = NULL;

    int found = FALSE;
    int i;
    GList *tmp_list;
  
    if (!samples) {
        if (!(samples = fopen("samples.dat", "a")))
            g_error ("Can't open 'samples.dat': %s", g_strerror(errno));
    }
  
    if (kselected.d[0] || kselected.d[1]) {
        for (i=0; i<num_guesses; i++) {
            if (WCHAR_EQ (kselected, kanjiguess[i]))
                found = TRUE;
        }
    }
  
    if (found)
        fprintf(samples,"%2x%2x %c%c\n", kselected.d[0], kselected.d[1],
           0x80 | kselected.d[0], 0x80 | kselected.d[1]);
    else {
        fprintf (samples, "0000 ??%d\n", unknownID);
        fprintf (stderr, "Unknown character saved, ID: %d\n", unknownID);
        unknownID++;
    }

    tmp_list = pad_area->strokes;
    while (tmp_list) {
        GList *stroke_list = tmp_list->data;
        while (stroke_list) {
            gint16 x = ((GdkPoint *)stroke_list->data)->x;
            gint16 y = ((GdkPoint *)stroke_list->data)->y;
            fprintf(samples, "%d %d ", x, y);
            stroke_list = stroke_list->next;
        }
        fprintf(samples, "\n");
        tmp_list = tmp_list->next;
    }
    fprintf(samples, "\n");
    fflush(samples);
}

void clear_callback(GtkWidget* w) {
    pad_area_clear (pad_area);
}

void look_up_callback(GtkWidget* w) {
/*	     kill 'HUP',$engine_pid; */
    GList *tmp_list;
    GString *message = g_string_new (NULL);
    GError *err = NULL;

    tmp_list = pad_area->strokes;
    while (tmp_list) {
        GList *stroke_list = tmp_list->data;
        while (stroke_list) {
            gint16 x = ((GdkPoint *)stroke_list->data)->x;
            gint16 y = ((GdkPoint *)stroke_list->data)->y;
            g_string_append_printf (message, "%d %d ", x, y);
            stroke_list = stroke_list->next;
        }
        g_string_append (message, "\n");
        tmp_list = tmp_list->next;
    }
    g_string_append (message, "\n");
    if (g_io_channel_write_chars (to_engine,
				message->str, message->len,
				NULL, &err) != G_IO_STATUS_NORMAL) {
        g_printerr ("Cannot write message to engine: %s\n", err->message);
        exit (1);
    }
    if (g_io_channel_flush (to_engine, &err) != G_IO_STATUS_NORMAL) {
        g_printerr ("Error flushing message to engine: %s\n", err->message);
        exit (1);
    }

    g_string_free (message, FALSE);
}

void append_jukugo_callback(GtkWidget* w) {
    if (kselected.d[0] || kselected.d[1]) {
        gchar* string_utf = utf8_for_char(kselected);
        
        GtkEntryBuffer* buffer = gtk_entry_get_buffer(GTK_ENTRY(jukugo_entry));
        gint len = gtk_entry_buffer_get_length(GTK_ENTRY_BUFFER(buffer));

        gtk_entry_buffer_insert_text(buffer, len, string_utf, -1);

        g_free(string_utf);

        clear_callback(w); // make ready for next input
    }
}

void query_jisho(GtkWidget* w) {
    // Take content from jukugo_entry and use it to search jisho
    if (gtk_entry_get_text_length(GTK_ENTRY(jukugo_entry)) == 0)
        return; // nothing to search for
    const gchar* searchTerm = gtk_entry_get_text(GTK_ENTRY(jukugo_entry));
    char* results = jisho_search_keyword(searchTerm);

    if (results == NULL) {
        g_printerr("Could not fetch data for query '%s' from jisho\n", searchTerm);
    }
    else {
        display_results(results);
        
        // add link to jisho
        // NOTE: Ideally I wouldn't have to add this every time, but this is a
        // quick fix, and it doesn't make for that bad UI.
        jishoLink = gtk_link_button_new("Open search in browser");
        gtk_box_pack_start(GTK_BOX(results_display_box), jishoLink, TRUE, TRUE, 0);
        gtk_widget_show(jishoLink);
        
        // construct a URL linking to the jisho.org search corresponding to the
        // query
        // NOTE: This is after the "display results" part because of ~the
        // display results function deleting everything in the results box and
        // me being too lazy to dive into that~ TECHNICAL REASONS
        char url[265]; // probably sufficient... like, what?
        strcpy(url, "http://www.jisho.org/search/");
        strncat(url, searchTerm, sizeof url - strlen(url) - 1); // not sure I need the -1 but whever
        gtk_link_button_set_uri(GTK_LINK_BUTTON(jishoLink), url);

        free(results);
    }
}

void clear_search_field(GtkWidget* w) {
    gtk_entry_set_text(GTK_ENTRY(jukugo_entry), "");
    gtk_widget_hide(scrollingResults); // hide results
}

void annotate_callback(GtkCheckMenuItem* menu_item, gpointer user_data) {
    //gboolean is_annotate = gtk_check_menu_item_get_active(menu_item);
    gboolean is_annotate = !pad_area->annotate;
    pad_area_set_annotate (pad_area, is_annotate);
}

void auto_look_up_callback(GtkCheckMenuItem* menu_item, gpointer user_data) {
    //gboolean is_auto_look_up = menu_item->active;
    gboolean is_auto_look_up = !pad_area->auto_look_up;
    pad_area_set_auto_look_up (pad_area, is_auto_look_up);
}

gboolean handle_keypress_callback(GtkWidget* widget, GdkEventKey* event, gpointer user_data) {
    if (event->state & GDK_CONTROL_MASK) {
        switch (event->keyval) {
            case GDK_q:
                exit_callback(widget);
                break;
            case GDK_c:
                copy_callback(widget);
                break;
            case GDK_x:
                clear_callback(widget);
                break;
            case GDK_z:
                undo_callback(widget);
                break;
            case GDK_l:
                look_up_callback(widget);
                break;
            case GDK_s:
                save_callback(widget);
                break;
            case GDK_a:
                append_jukugo_callback(widget);
            default:
                return FALSE;
        }
    }
    return FALSE;
}

gboolean jukugo_keypress_callback(GtkWidget* widget, GdkEventKey* event, gpointer user_data) {
    switch(event->keyval) {
        case GDK_Return:
            query_jisho(widget);
            break;
    }

    return FALSE;
}

void pad_area_changed_callback (PadArea *area)
{
  update_sensitivity ();
  if(area->auto_look_up)
    {
      look_up_callback (NULL);
    }
}
