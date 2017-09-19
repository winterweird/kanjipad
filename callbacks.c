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
#include "keyboard.h" // keyboard_set_visible

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

void insert_char_callback(GtkWidget* w, gpointer user_data) {
    char ch = (char) user_data;
    char chstr[2] = {0};
    chstr[0] = ch;
    
    // get current position, insert text, and set the position to the updated
    // point
    int pos = gtk_editable_get_position(GTK_EDITABLE(jukugo_entry));
    gtk_editable_insert_text(GTK_EDITABLE(jukugo_entry), chstr, 1, &pos);
    gtk_editable_set_position(GTK_EDITABLE(jukugo_entry), pos);
}

void delete_char_callback(GtkWidget* w, gpointer user_data) {
    // delete selected text if text is selected
    // if not, get current position, delete char in front of it
    GtkEditable* entry = GTK_EDITABLE(jukugo_entry);
    int start, end;
    if (!gtk_editable_get_selection_bounds(entry, &start, &end)) {
        end = gtk_editable_get_position(entry);
        start = end-1;
    }
    if (end == 0) return; // can't delete text before the first character
    else {
        gtk_editable_delete_text(entry, start, end);
    }
}

void cursor_pos_left_callback(GtkWidget* w) {
    int pos = gtk_editable_get_position(GTK_EDITABLE(jukugo_entry));
    if (pos > 0) pos--;
    gtk_editable_set_position(GTK_EDITABLE(jukugo_entry), pos);
}

void cursor_pos_right_callback(GtkWidget* w) {
    int pos = gtk_editable_get_position(GTK_EDITABLE(jukugo_entry));

    // get endpos
    // NOTE: this should be easier than such...
    gtk_editable_set_position(GTK_EDITABLE(jukugo_entry), -1);
    int endPos = gtk_editable_get_position(GTK_EDITABLE(jukugo_entry));
    
        if (pos < endPos) pos++;
        gtk_editable_set_position(GTK_EDITABLE(jukugo_entry), pos);
}

// helper function: compare at most n characters, or until either one is null
int strncomp(const char* a, const char* b, int maxlen) {
    int i = 0;
    while (*a != 0 || *b != 0) {
        if (*a != *b) return 1;
        a++; b++; i++;
        if (i == maxlen) break;
    }
    return 0;
}

// conversion table
static const char* matchesHiragana(const char* ch, int len) {
    if (strcmp(ch, "xa") == 0) { return "\xe3\x81\x81"; /* hiragana small a */ }
    if (strcmp(ch, "a") == 0) { return "\xe3\x81\x82"; /* hiragana a */ }
    if (strcmp(ch, "xi") == 0) { return "\xe3\x81\x83"; /* hiragana small i */ }
    if (strcmp(ch, "i") == 0) { return "\xe3\x81\x84"; /* hiragana i */ }
    if (strcmp(ch, "xu") == 0) { return "\xe3\x81\x85"; /* hiragana small u */ }
    if (strcmp(ch, "u") == 0) { return "\xe3\x81\x86"; /* hiragana u */ }
    if (strcmp(ch, "wu") == 0) { return "\xe3\x81\x86\xe3\x81\x85"; /* hiragana wu */ }
    if (strcmp(ch, "wi") == 0) { return "\xe3\x81\x86\xe3\x81\x83"; /* hiragana wi */ }
    if (strcmp(ch, "we") == 0) { return "\xe3\x81\x86\xe3\x81\x87"; /* hiragana we */ }
    if (strcmp(ch, "xe") == 0) { return "\xe3\x81\x87"; /* hiragana small e */ }
    if (strcmp(ch, "e") == 0) { return "\xe3\x81\x88"; /* hiragana e */ }
    if (strcmp(ch, "xo") == 0) { return "\xe3\x81\x89"; /* hiragana small o */ }
    if (strcmp(ch, "o") == 0) { return "\xe3\x81\x8a"; /* hiragana o */ }
    if (strcmp(ch, "kk") == 0) { return "\xe3\x81\xa3" "k"; /* hiragana kk */ }
    if (strcmp(ch, "gg") == 0) { return "\xe3\x81\xa3" "g"; /* hiragana gg */ }
    if (strcmp(ch, "ka") == 0) { return "\xe3\x81\x8b"; /* hiragana ka */ }
    if (strcmp(ch, "ga") == 0) { return "\xe3\x81\x8c"; /* hiragana ga */ }
    if (strcmp(ch, "ki") == 0) { return "\xe3\x81\x8d"; /* hiragana ki */ }
    if (strcmp(ch, "kya") == 0) { return "\xe3\x81\x8d\xe3\x82\x83"; /* hiragana kya */ }
    if (strcmp(ch, "kyu") == 0) { return "\xe3\x81\x8d\xe3\x82\x85"; /* hiragana kyu */ }
    if (strcmp(ch, "kyo") == 0) { return "\xe3\x81\x8d\xe3\x82\x87"; /* hiragana kyo */ }
    if (strcmp(ch, "gi") == 0) { return "\xe3\x81\x8e"; /* hiragana gi */ }
    if (strcmp(ch, "gya") == 0) { return "\xe3\x81\x8e\xe3\x82\x83"; /* hiragana gya */ }
    if (strcmp(ch, "gyu") == 0) { return "\xe3\x81\x8e\xe3\x82\x85"; /* hiragana gyu */ }
    if (strcmp(ch, "gyo") == 0) { return "\xe3\x81\x8e\xe3\x82\x87"; /* hiragana gyo */ }
    if (strcmp(ch, "ku") == 0) { return "\xe3\x81\x8f"; /* hiragana ku */ }
    if (strcmp(ch, "gu") == 0) { return "\xe3\x81\x90"; /* hiragana gu */ }
    if (strcmp(ch, "ke") == 0) { return "\xe3\x81\x91"; /* hiragana ke */ }
    if (strcmp(ch, "ge") == 0) { return "\xe3\x81\x92"; /* hiragana ge */ }
    if (strcmp(ch, "ko") == 0) { return "\xe3\x81\x93"; /* hiragana ko */ }
    if (strcmp(ch, "go") == 0) { return "\xe3\x81\x94"; /* hiragana go */ }
    if (strcmp(ch, "ss") == 0) { return "\xe3\x81\xa3" "s"; /* hiragana ss */ }
    if (strcmp(ch, "zz") == 0) { return "\xe3\x81\xa3" "z"; /* hiragana zz */ }
    if (strcmp(ch, "jj") == 0) { return "\xe3\x81\xa3" "j"; /* hiragana jj */ }
    if (strcmp(ch, "sa") == 0) { return "\xe3\x81\x95"; /* hiragana sa */ }
    if (strcmp(ch, "za") == 0) { return "\xe3\x81\x96"; /* hiragana za */ }
    if (strcmp(ch, "shi") == 0) { return "\xe3\x81\x97"; /* hiragana shi */ }
    if (strcmp(ch, "si") == 0) { return "\xe3\x81\x97"; /* hiragana shi */ }
    if (strcmp(ch, "sha") == 0) { return "\xe3\x81\x97\xe3\x82\x83"; /* hiragana sha */ }
    if (strcmp(ch, "shu") == 0) { return "\xe3\x81\x97\xe3\x82\x85"; /* hiragana shu */ }
    if (strcmp(ch, "sho") == 0) { return "\xe3\x81\x97\xe3\x82\x87"; /* hiragana sho */ }
    if (strcmp(ch, "ji") == 0) { return "\xe3\x81\x98"; /* hiragana ji */ }
    if (strcmp(ch, "ja") == 0) { return "\xe3\x81\x98\xe3\x82\x83"; /* hiragana ja */ }
    if (strcmp(ch, "ju") == 0) { return "\xe3\x81\x98\xe3\x82\x85"; /* hiragana ju */ }
    if (strcmp(ch, "jo") == 0) { return "\xe3\x81\x98\xe3\x82\x87"; /* hiragana jo */ }
    if (strcmp(ch, "su") == 0) { return "\xe3\x81\x99"; /* hiragana su */ }
    if (strcmp(ch, "zu") == 0) { return "\xe3\x81\x9a"; /* hiragana zu */ }
    if (strcmp(ch, "se") == 0) { return "\xe3\x81\x9b"; /* hiragana se */ }
    if (strcmp(ch, "ze") == 0) { return "\xe3\x81\x9c"; /* hiragana ze */ }
    if (strcmp(ch, "so") == 0) { return "\xe3\x81\x9d"; /* hiragana so */ }
    if (strcmp(ch, "zo") == 0) { return "\xe3\x81\x9e"; /* hiragana zo */ }
    if (strcmp(ch, "tt") == 0) { return "\xe3\x81\xa3" "t"; /* hiragana tt */ }
    if (strcmp(ch, "dd") == 0) { return "\xe3\x81\xa3" "d"; /* hiragana dd */ }
    if (strcmp(ch, "cc") == 0) { return "\xe3\x81\xa3" "c"; /* hiragana dd */ }
    if (strcmp(ch, "ta") == 0) { return "\xe3\x81\x9f"; /* hiragana ta */ }
    if (strcmp(ch, "da") == 0) { return "\xe3\x81\xa0"; /* hiragana da */ }
    if (strcmp(ch, "chi") == 0) { return "\xe3\x81\xa1"; /* hiragana chi */ }
    if (strcmp(ch, "cha") == 0) { return "\xe3\x81\xa1\xe3\x82\x83"; /* hiragana cha */ }
    if (strcmp(ch, "chu") == 0) { return "\xe3\x81\xa1\xe3\x82\x85"; /* hiragana chu */ }
    if (strcmp(ch, "cho") == 0) { return "\xe3\x81\xa1\xe3\x82\x87"; /* hiragana cho */ }
    if (strcmp(ch, "dji") == 0) { return "\xe3\x81\xa2"; /* hiragana (d)ji */ }
    if (strcmp(ch, "xtsu") == 0) { return "\xe3\x81\xa3"; /* hiragana small tsu */ }
    if (strcmp(ch, "tsu") == 0) { return "\xe3\x81\xa4"; /* hiragana tsu */ }
    if (strcmp(ch, "dzu") == 0) { return "\xe3\x81\xa5"; /* hiragana dzu */ }
//    if (strcmp(ch, "du") == 0) { return "\xe3\x81\xa5"; /* hiragana dzu */ }
    if (strcmp(ch, "te") == 0) { return "\xe3\x81\xa6"; /* hiragana te */ }
    if (strcmp(ch, "ti") == 0) { return "\xe3\x81\xa6\xe3\x81\x83"; /* hiragana ti */ }
    if (strcmp(ch, "de") == 0) { return "\xe3\x81\xa7"; /* hiragana de */ }
    if (strcmp(ch, "di") == 0) { return "\xe3\x81\xa7\xe3\x81\x83"; /* hiragana di */ }
    if (strcmp(ch, "to") == 0) { return "\xe3\x81\xa8"; /* hiragana to */ }
    if (strcmp(ch, "tu") == 0) { return "\xe3\x81\xa8\xe3\x81\x85"; /* hiragana tu */ }
    if (strcmp(ch, "do") == 0) { return "\xe3\x81\xa9"; /* hiragana do */ }
    if (strcmp(ch, "du") == 0) { return "\xe3\x81\xa9\xe3\x81\x85"; /* hiragana du */ }
    if (strcmp(ch, "na") == 0) { return "\xe3\x81\xaa"; /* hiragana na */ }
    if (strcmp(ch, "ni") == 0) { return "\xe3\x81\xab"; /* hiragana ni */ }
    if (strcmp(ch, "nya") == 0) { return "\xe3\x81\xab\xe3\x82\x83"; /* hiragana nya */ }
    if (strcmp(ch, "nyu") == 0) { return "\xe3\x81\xab\xe3\x82\x85"; /* hiragana nyu */ }
    if (strcmp(ch, "nyo") == 0) { return "\xe3\x81\xab\xe3\x82\x87"; /* hiragana nyo */ }
    if (strcmp(ch, "nu") == 0) { return "\xe3\x81\xac"; /* hiragana nu */ }
    if (strcmp(ch, "ne") == 0) { return "\xe3\x81\xad"; /* hiragana ne */ }
    if (strcmp(ch, "no") == 0) { return "\xe3\x81\xae"; /* hiragana no */ }
    if (strcmp(ch, "hh") == 0) { return "\xe3\x81\xa3" "h"; /* hiragana hh */ }
    if (strcmp(ch, "bb") == 0) { return "\xe3\x81\xa3" "b"; /* hiragana bb */ }
    if (strcmp(ch, "pp") == 0) { return "\xe3\x81\xa3" "p"; /* hiragana pp */ }
    if (strcmp(ch, "ff") == 0) { return "\xe3\x81\xa3" "f"; /* hiragana ff */ }
    if (strcmp(ch, "ha") == 0) { return "\xe3\x81\xaf"; /* hiragana ha */ }
    if (strcmp(ch, "ba") == 0) { return "\xe3\x81\xb0"; /* hiragana ba */ }
    if (strcmp(ch, "pa") == 0) { return "\xe3\x81\xb1"; /* hiragana pa */ }
    if (strcmp(ch, "hi") == 0) { return "\xe3\x81\xb2"; /* hiragana hi */ }
    if (strcmp(ch, "hya") == 0) { return "\xe3\x81\xb2\xe3\x82\x83"; /* hiragana hya */ }
    if (strcmp(ch, "hyu") == 0) { return "\xe3\x81\xb2\xe3\x82\x85"; /* hiragana hyu */ }
    if (strcmp(ch, "hyo") == 0) { return "\xe3\x81\xb2\xe3\x82\x87"; /* hiragana hyo */ }
    if (strcmp(ch, "bi") == 0) { return "\xe3\x81\xb3"; /* hiragana bi */ }
    if (strcmp(ch, "bya") == 0) { return "\xe3\x81\xb3\xe3\x82\x83"; /* hiragana bya */ }
    if (strcmp(ch, "byu") == 0) { return "\xe3\x81\xb3\xe3\x82\x85"; /* hiragana byu */ }
    if (strcmp(ch, "byo") == 0) { return "\xe3\x81\xb3\xe3\x82\x87"; /* hiragana byo */ }
    if (strcmp(ch, "pi") == 0) { return "\xe3\x81\xb4"; /* hiragana pi */ }
    if (strcmp(ch, "pya") == 0) { return "\xe3\x81\xb4\xe3\x82\x83"; /* hiragana pya */ }
    if (strcmp(ch, "pyu") == 0) { return "\xe3\x81\xb4\xe3\x82\x85"; /* hiragana pyu */ }
    if (strcmp(ch, "pyo") == 0) { return "\xe3\x81\xb4\xe3\x82\x87"; /* hiragana pyo */ }
    if (strcmp(ch, "fu") == 0) { return "\xe3\x81\xb5"; /* hiragana fu */ }
    if (strcmp(ch, "fa") == 0) { return "\xe3\x81\xb5\xe3\x81\x81"; /* hiragana fa */ }
    if (strcmp(ch, "fi") == 0) { return "\xe3\x81\xb5\xe3\x81\x83"; /* hiragana fi */ }
    if (strcmp(ch, "fe") == 0) { return "\xe3\x81\xb5\xe3\x81\x87"; /* hiragana fe */ }
    if (strcmp(ch, "fo") == 0) { return "\xe3\x81\xb5\xe3\x81\x89"; /* hiragana fo */ }
    if (strcmp(ch, "bu") == 0) { return "\xe3\x81\xb6"; /* hiragana bu */ }
    if (strcmp(ch, "pu") == 0) { return "\xe3\x81\xb7"; /* hiragana pu */ }
    if (strcmp(ch, "he") == 0) { return "\xe3\x81\xb8"; /* hiragana he */ }
    if (strcmp(ch, "be") == 0) { return "\xe3\x81\xb9"; /* hiragana be */ }
    if (strcmp(ch, "pe") == 0) { return "\xe3\x81\xba"; /* hiragana pe */ }
    if (strcmp(ch, "ho") == 0) { return "\xe3\x81\xbb"; /* hiragana ho */ }
    if (strcmp(ch, "bo") == 0) { return "\xe3\x81\xbc"; /* hiragana bo */ }
    if (strcmp(ch, "po") == 0) { return "\xe3\x81\xbd"; /* hiragana po */ }
    if (strcmp(ch, "mm") == 0) { return "\xe3\x81\xa3" "m"; /* hiragana mm */ }
    if (strcmp(ch, "ma") == 0) { return "\xe3\x81\xbe"; /* hiragana ma */ }
    if (strcmp(ch, "mi") == 0) { return "\xe3\x81\xbf"; /* hiragana mi */ }
    if (strcmp(ch, "mya") == 0) { return "\xe3\x81\xbf\xe3\x82\x83"; /* hiragana mya */ }
    if (strcmp(ch, "myu") == 0) { return "\xe3\x81\xbf\xe3\x82\x85"; /* hiragana myu */ }
    if (strcmp(ch, "myo") == 0) { return "\xe3\x81\xbf\xe3\x82\x87"; /* hiragana myo */ }
    if (strcmp(ch, "mu") == 0) { return "\xe3\x82\x80"; /* hiragana mu */ }
    if (strcmp(ch, "me") == 0) { return "\xe3\x82\x81"; /* hiragana me */ }
    if (strcmp(ch, "mo") == 0) { return "\xe3\x82\x82"; /* hiragana mo */ }
    if (strcmp(ch, "yy") == 0) { return "\xe3\x81\xa3" "y"; /* hiragana yy */ }
    if (strcmp(ch, "xya") == 0) { return "\xe3\x82\x83"; /* hiragana small ya */ }
    if (strcmp(ch, "ya") == 0) { return "\xe3\x82\x84"; /* hiragana ya */ }
    if (strcmp(ch, "xyu") == 0) { return "\xe3\x82\x85"; /* hiragana small yu */ }
    if (strcmp(ch, "yu") == 0) { return "\xe3\x82\x86"; /* hiragana yu */ }
    if (strcmp(ch, "xyo") == 0) { return "\xe3\x82\x87"; /* hiragana small yo */ }
    if (strcmp(ch, "yo") == 0) { return "\xe3\x82\x88"; /* hiragana yo */ }
    if (strcmp(ch, "rr") == 0) { return "\xe3\x81\xa3" "r"; /* hiragana rr */ }
    if (strcmp(ch, "ra") == 0) { return "\xe3\x82\x89"; /* hiragana ra */ }
    if (strcmp(ch, "ri") == 0) { return "\xe3\x82\x8a"; /* hiragana ri */ }
    if (strcmp(ch, "rya") == 0) { return "\xe3\x82\x8a\xe3\x82\x83"; /* hiragana rya */ }
    if (strcmp(ch, "ryu") == 0) { return "\xe3\x82\x8a\xe3\x82\x85"; /* hiragana ryu */ }
    if (strcmp(ch, "ryo") == 0) { return "\xe3\x82\x8a\xe3\x82\x87"; /* hiragana ryo */ }
    if (strcmp(ch, "ru") == 0) { return "\xe3\x82\x8b"; /* hiragana ru */ }
    if (strcmp(ch, "re") == 0) { return "\xe3\x82\x8c"; /* hiragana re */ }
    if (strcmp(ch, "ro") == 0) { return "\xe3\x82\x8d"; /* hiragana ro */ }
    if (strcmp(ch, "ww") == 0) { return "\xe3\x81\xa3" "w"; /* hiragana ww */ }
    if (strcmp(ch, "xwa") == 0) { return "\xe3\x82\x8e"; /* hiragana small wa */ }
    if (strcmp(ch, "wa") == 0) { return "\xe3\x82\x8f"; /* hiragana wa */ }
    if (strcmp(ch, "wo") == 0) { return "\xe3\x82\x92"; /* hiragana wo */ }
    if (strcmp(ch, "nn") == 0) { return "\xe3\x82\x93" "n"; /* hiragana nn */ }
    if (strcmp(ch, "n ") == 0) { return "\xe3\x82\x93"; /* hiragana n */ }
    if (strcmp(ch, "vv") == 0) { return "\xe3\x81\xa3" "v"; /* hiragana vv */ }
    if (strcmp(ch, "vu") == 0) { return "\xe3\x82\x94"; /* hiragana vu */ }
    if (strcmp(ch, "va") == 0) { return "\xe3\x82\x94\xe3\x81\x81"; /* hiragana va */ }
    if (strcmp(ch, "vi") == 0) { return "\xe3\x82\x94\xe3\x81\x83"; /* hiragana vi */ }
    if (strcmp(ch, "ve") == 0) { return "\xe3\x82\x94\xe3\x81\x87"; /* hiragana ve */ }
    if (strcmp(ch, "vo") == 0) { return "\xe3\x82\x94\xe3\x81\x89"; /* hiragana vo */ }
    if (strcmp(ch, "xka") == 0) { return "\xe3\x82\x95"; /* hiragana small ka */ }
    if (strcmp(ch, "xke") == 0) { return "\xe3\x82\x96"; /* hiragana small ke */ }

    return NULL;
}

void replace_hiragana_callback() {
    if (!hiragana_replace) return; // do nothing
    
    char* content = gtk_editable_get_chars(GTK_EDITABLE(jukugo_entry), 0, -1);
    int len = 0;
    int buflen = 256; // should be plenty
    char* buf = calloc(buflen, sizeof *buf);
    int i = 0; // buf index

    for (const char* ch = content; *ch != 0; len++) {
        const char* match = matchesHiragana(ch, len);
        if (match == NULL) {
            if (len == 2) {
                buf[i++] = *ch;
                len = 0;
                ch++;
            }
        }
        else {
            strcat(buf, match);
            i = strlen(buf)+1;
            ch += strlen(match);
            len = 0;
        }
    }
    
    int insertPos = 0;
    // block signal handler to avoid recursive callback
    g_signal_handler_block(jukugo_entry, jukugo_entry_insert_text_handler_id);
    // delete text and replace with buffered stuff
    gtk_editable_delete_text(GTK_EDITABLE(jukugo_entry), 0, -1);
    gtk_editable_insert_text(GTK_EDITABLE(jukugo_entry), buf, strlen(buf), &insertPos);
    // unblock signal handler
    g_signal_handler_unblock(jukugo_entry, jukugo_entry_insert_text_handler_id);
//    gtk_editable_insert_text(GTK_EDITABLE(jukugo_entry), buf, nChars-1, &insertPos);

    free(buf);
}

void show_keyboard_callback(GtkWidget* w) {
    // programmatically activate checkmenuitem
    // This calls callback, which sets keyboard visibility
    GtkWidget* kbdCheckItem = gtk_ui_manager_get_widget(menu_manager, "/ui/MainMenu/CharacterMenu/Keyboard");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(kbdCheckItem), TRUE);
}

void hide_keyboard_callback(GtkWidget* w) {
    // programmatically deactivate checkmenuitem
    // This calls callback, which sets keyboard visibility
    GtkWidget* kbdCheckItem = gtk_ui_manager_get_widget(menu_manager, "/ui/MainMenu/CharacterMenu/Keyboard");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(kbdCheckItem), FALSE);
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

void toggle_keyboard_callback(GtkCheckMenuItem* menu_item, gpointer user_data) {
    gboolean keyboard_visible = !keyboard.isVisible;
    keyboard_set_visible(keyboard_visible);
}

void toggle_hiragana_replace_callback(GtkCheckMenuItem* menu_item, gpointer user_data) {
    hiragana_replace = !hiragana_replace; // makin' it simple
    if (hiragana_replace) {
        replace_hiragana_callback();
    }
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
