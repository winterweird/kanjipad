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

/** Refactored and modified by Vegard Itland (2017)
 *
 *  NOTE: This is an original feature I (Vegard) am developing. It's based on
 *  the RESTful API of jisho.org, an online Japanese dictionary. In order to
 *  parse the JSON results returned from jisho.org, I am utilizing a JSON
 *  validating/parsing library called "YAJL" (Yet Another JSON Library). Much
 *  thanks to the creator! Check it out at http://lloyd.github.io/yajl/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_to_gtk.h"
#include "yajl/yajl_tree.h"
#include "global_vars.h"


// Helper method: return kanji with reading in fancy parentheses
// NOTE: allocates memory - call free on return value
// NOTE: if kanji is NULL (and reading isn't), set "kanji" to be reading
static char* withReading(const char* kanji, const char* reading) {
    if (kanji == NULL) {
        // reading is guaranteed to not simultaneously be NULL
        kanji = reading; // pretend reading is kanji
        reading = ""; // we have no use for the reading now!
    }
    else if (reading == NULL) {
        reading = ""; // not sure why this makes a difference, but whatever.
    }
    char* ret = malloc(strlen(kanji) + strlen(reading) + strlen(" \xe3\x80\x90\xe3\x80\x91") + 1);
    strcpy(ret, kanji);
    if (strlen(reading) != 0) {
        strcat(ret, " \xe3\x80\x90"); // left black lenticular bracket
        strcat(ret, reading);
        strcat(ret, "\xe3\x80\x91"); // right black lenticular bracket
    }
    return ret;
}

// Helper method: easier way to get JSON value
static yajl_val yajl_get_soft(yajl_val root, const char* id, yajl_type type) {
    const char* idPath[] = {id, 0};
    return yajl_tree_get(root, idPath, type);
}

// Helper method: easier way to get and check JSON value
static yajl_val yajl_get(yajl_val root, const char* id, yajl_type type) {
    yajl_val ret = yajl_get_soft(root, id, type);
    if (ret == NULL) {
        g_printerr("Path not found: %s\n", id);
        exit(1);
    }
    return ret;
}

// Helper method: simpler way to add aligned table
// NOTE: I'm assuming that the arguments are "x align" and "y align" (haven't
// checked)
static void table_addlabel_aligned(GtkWidget* w, const char* text, int row, int col, double xalign, double yalign) {
    GtkWidget* label = gtk_label_new(text);
    gtk_misc_set_alignment(GTK_MISC(label), xalign, yalign);
    
    // make labels wrap
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    // the "subtract 10" is to avoid the scrollbar overlapping, the -1 means
    // that the height is automatically caluclated
    gtk_widget_set_size_request(label, WINDOW_WIDTH/2 - 10, -1);
    
    gtk_table_attach_defaults(GTK_TABLE(w), label, col, col+1, row, row+1);
    gtk_widget_show(label);
}

// Helper method: simpler way to add label to table (alignment defaults to 0, 0.5)
static void table_addlabel(GtkWidget* w, const char* text, int row, int col) {
    table_addlabel_aligned(w, text, row, col, 0, 0.5);
}

static char* semicat(char* prefix, const char* suffix) {
    prefix = realloc(prefix, strlen(prefix) + strlen(suffix) + 3);
    strcat(prefix, "; ");
    strcat(prefix, suffix);
    return prefix;
}

// Helper method: does weird shit and makes it work
char* getmeanings(yajl_val entry) {
    yajl_val engdefArr = yajl_get(entry, "english_definitions", yajl_t_array);

    int arrlen = YAJL_GET_ARRAY(engdefArr)->len;
    
    char* meaningList;
    yajl_val d = YAJL_GET_ARRAY(engdefArr)->values[0]; // assuming there is at least one meaning
    const char* first = YAJL_GET_STRING(d);
    meaningList = malloc(strlen(first) + 1);
    strcpy(meaningList, first);
    
    for (int j = 1; j < arrlen; j++) {
        d = YAJL_GET_ARRAY(engdefArr)->values[j];
        meaningList = semicat(meaningList, YAJL_GET_STRING(d));
    }
    
    return meaningList;
}

// Helper method: does even weirder shit and makes it work
GtkWidget* getforms(yajl_val entry, int from, int to) {
    if (to == -1) {
        to = YAJL_GET_ARRAY(entry)->len;
    }
    
    GtkWidget* otherFormTable = gtk_table_new(to-from, 1, FALSE);
    
    for (int i = from; i < to; i++) {
        yajl_val word = YAJL_GET_ARRAY(entry)->values[i];
        yajl_val w = yajl_get_soft(word, "word", yajl_t_string);
        yajl_val r = yajl_get_soft(word, "reading", yajl_t_string);
        if (w == NULL && r == NULL) {
            g_printerr("Error: neither word nor reading found; skipping\n");
            continue;
        }

        char* kanjiWithReading = withReading(YAJL_GET_STRING(w), YAJL_GET_STRING(r));

        table_addlabel(otherFormTable, kanjiWithReading, i, 0);

        free(kanjiWithReading);
    }
    return otherFormTable;
}

// Helper method: create the layout for a widget (from a JSON object node)
static GtkWidget* displayEntry(yajl_val entryElement) {
    if (entryElement == NULL) {
        // sanity check
        g_printerr("Error: no element\n");
        exit(1);
    }

    GtkWidget* entrytable;

    // create table
    entrytable = gtk_table_new(6, 2, FALSE);
    gtk_box_pack_start(GTK_BOX(results_display_box), entrytable, TRUE, TRUE, 0);
    gtk_widget_show(entrytable);

    
    yajl_val japaneseArr = yajl_get(entryElement, "japanese", yajl_t_array);
    
    if (YAJL_GET_ARRAY(japaneseArr)->len == 0) {
        g_printerr("Array 'japanese' is empty\n");
    }
    else {
        yajl_val word = YAJL_GET_ARRAY(japaneseArr)->values[0];
        
        // get kanji (or word or whatever)
        yajl_val w = yajl_get_soft(word, "word", yajl_t_string);
        
        // get reading
        yajl_val r = yajl_get_soft(word, "reading", yajl_t_string);
        
        // get kanji with reading in fancy dictionary parentheses
        char* kanjiWithReading = withReading(YAJL_GET_STRING(w), YAJL_GET_STRING(r));
        table_addlabel(entrytable, kanjiWithReading, 0, 0);
        free(kanjiWithReading);
        
        // handle whether word is common
        yajl_val iscommon = yajl_get_soft(entryElement, "is_common", yajl_t_any);
        if (iscommon != NULL && YAJL_IS_TRUE(iscommon)) {
            table_addlabel(entrytable, "Common word", 0, 1);
        }
        // add some space
        table_addlabel(entrytable, "", 1, 0);

        // add stuff at row 2, column 0, and at row 2, column 1,
        // "other forms"-style!
        table_addlabel_aligned(entrytable, "Meanings:", 2, 0, 0, 0);

        yajl_val senses = yajl_get(entryElement, "senses", yajl_t_array);
        int senselen = YAJL_GET_ARRAY(senses)->len;
        GtkWidget* sensetab = gtk_table_new(senselen, 1, FALSE);
        gtk_widget_show(sensetab);

        for (int i = 0; i < senselen; i++) {
            yajl_val sense = YAJL_GET_ARRAY(senses)->values[i];

            char* meanings = getmeanings(sense);
            table_addlabel(sensetab, meanings, i, 0);
            free(meanings);
        }

        gtk_table_attach_defaults(GTK_TABLE(entrytable), sensetab, 1, 2, 2, 3);
        
        // add some space
        table_addlabel(entrytable, "", 3, 0);
        
        // display other forms
        table_addlabel_aligned(entrytable, "Other forms:", 4, 0, 0, 0);

        GtkWidget* otherForms = getforms(japaneseArr, 1, -1);
//        GtkWidget* otherFormTable = gtk_table_new(YAJL_GET_ARRAY(japaneseArr)->len - 1, 1, FALSE);
        gtk_table_attach_defaults(GTK_TABLE(entrytable), otherForms, 1, 2, 4, 5);
        gtk_widget_show(otherForms);
        
        
        GtkWidget* hsep = gtk_hseparator_new();
        gtk_table_attach_defaults(GTK_TABLE(entrytable), hsep, 0, 2, 5, 6);
        gtk_widget_show(hsep);
    }
    return entrytable;
}

void display_results(const char* results) {
    printf("Results:\n%s\n", results); // maybe I should remove at a point

    // Fair warning: I'll be making heaps of temporary variables just to ensure
    // I have my shit straight. Also, I'll be making liberal exit calls just to
    // ensure that I don't fuck up on the JSON structure. In the end
    // application, these shouldn't really ever actually exit - I'm
    // contemplating adding in some "ifdef DEBUG" or whatever it is and disable
    // that if I'm ever making a "release build" - although I am not ever making
    // that, come to think of it...
        
    char errbuf[1024];
    errbuf[0] = 0; // null-plug errbuf - YAJL example does, so I'm not about to not
    yajl_val root_node = yajl_tree_parse(results, errbuf, sizeof errbuf);

    if (root_node == NULL) {
        // YAJL parse error - print an error and exit
        g_printerr("Parse error: %s\n", errbuf);
        exit(1);
    }

    // clean results box
    GList *children;
    children = gtk_container_get_children(GTK_CONTAINER(results_display_box));
    for (GList* it = children; it != NULL; it = g_list_next(it)) {
        gtk_container_remove(GTK_CONTAINER(results_display_box), GTK_WIDGET(it->data));
//        gtk_widget_destroy(GTK_WIDGET(it->data));
    }
    g_list_free(children);
    
    gtk_widget_show(scrollingResults); // show results box
    
    // find data
    const char* datPath[] = {"data", 0}; // look at datPath!
    yajl_val dat = yajl_tree_get(root_node, datPath, yajl_t_array);
    if (dat == NULL) {
        // probably error in jisho.org API (but that means there's been an
        // update! Whohoo!)
        g_printerr("Path not found: data (array)\n");
        exit(1);
    }
    else if (YAJL_GET_ARRAY(dat)->len == 0) {
        // data list was empty
        GtkWidget* nodata = gtk_label_new("No data found.");
        gtk_box_pack_start(GTK_BOX(results_display_box), nodata, TRUE, TRUE, 0);
        gtk_widget_show(nodata);
        return;
    }

    GtkWidget* entries;
    entries = gtk_table_new(YAJL_GET_ARRAY(dat)->len, 1, FALSE);
    gtk_box_pack_start(GTK_BOX(results_display_box), entries, TRUE, TRUE, 0);
    gtk_widget_show(entries);

    for (int i = 0; i < YAJL_GET_ARRAY(dat)->len; i++) {
        GtkWidget* entry = displayEntry(YAJL_GET_ARRAY(dat)->values[i]);
        // for some reason, removing this line fixes the weird error messages
//        gtk_table_attach_defaults(GTK_TABLE(entries), entry, 0, 1, i, i+1);
        gtk_widget_show(entry);
    } 
    
    yajl_tree_free(root_node);
}
