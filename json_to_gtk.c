#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_to_gtk.h"
#include "yajl/yajl_tree.h"
#include "global_vars.h"

void display_results(const char* results) {
    // TODO: figure out
    printf("Results:\n%s\n", results);

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

    const char* datPath[] = {"data", 0}; // look at datPath!
    yajl_val dat = yajl_tree_get(root_node, datPath, yajl_t_array);

    if (dat == NULL) {
        g_printerr("Path not found: data (array)\n");
        exit(1);
    }
    else if (YAJL_GET_ARRAY(dat)->len == 0) {
        GtkWidget* nodata = gtk_label_new("No data found.");
        gtk_box_pack_start(GTK_BOX(results_display_box), nodata, TRUE, TRUE, 0);
        gtk_widget_show(nodata);
        return;
    }
    
    GtkWidget* entries;
    entries = gtk_table_new(YAJL_GET_ARRAY(dat)->len, 3, FALSE);
    gtk_box_pack_start(GTK_BOX(results_display_box), entries, TRUE, TRUE, 0);
    gtk_widget_show(entries);

    for (int i = 0; i < YAJL_GET_ARRAY(dat)->len; i++) {
        GtkWidget* label;
        
        yajl_val datLmnt = YAJL_GET_ARRAY(dat)->values[i];
        if (datLmnt == NULL) {
            g_printerr("Path not found: element %d in data (array)\n", i);
            exit(1);
        }

        const char* japPath[] = {"japanese", 0};
        yajl_val japaneseArr = yajl_tree_get(datLmnt, japPath, yajl_t_array);
        if (japaneseArr == NULL) {
            g_printerr("Path not found: japanese (array) in element %d in data (array)\n", i);
            exit(1);
        }
        if (YAJL_GET_ARRAY(japaneseArr)->len == 0) {
            g_printerr("Array 'japanese' is empty\n");
        }
        else {
            const char* wordPath[] = {"word", 0};
            yajl_val mainWord = YAJL_GET_ARRAY(japaneseArr)->values[0];
            yajl_val w = yajl_tree_get(mainWord, wordPath, yajl_t_string);
            if (w == NULL) {
                g_printerr("Path not found: word (string) in element 0 in japanese (array) in element %d in data (array)\n", i);
                exit(1);
            }
            label = gtk_label_new(YAJL_GET_STRING(w));
            gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
            gtk_table_attach_defaults(GTK_TABLE(entries), label, 0, 1, i, i+1);
            gtk_widget_show(label);

            const char* readPath[] = {"reading", 0};
            yajl_val r = yajl_tree_get(mainWord, readPath, yajl_t_string);
            if (r == NULL) {
                g_printerr("Path not found: reading (string) in element 0 in japanese (array) in element %d in data(array)\n", i);
                exit(1);
            }
            
            // print reading in parentheses
            const char* str = YAJL_GET_STRING(r);
            char* rstring = malloc(strlen(str) + 3);
            strcpy(rstring, "(");
            strcat(rstring, str);
            strcat(rstring, ")");
            label = gtk_label_new(rstring); // build label
            free(rstring);
            
            // put label in table
            gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
            gtk_table_attach_defaults(GTK_TABLE(entries), label, 1, 2, i, i+1);
            gtk_widget_show(label);
        }
        
        // handle whether word is common
        const char* iscmnPath[] = {"is_common", 0};
        yajl_val iscommon = yajl_tree_get(datLmnt, iscmnPath, yajl_t_any);
        if (iscommon == NULL) {
            g_printerr("Path not found: is_common (any) in element in data (array)\n");
            exit(1);
        }

        // Add label denoting whether or not word is common
        if (YAJL_IS_TRUE(iscommon)) {
            printf("True\n");
            label = gtk_label_new("(C)");
        }
        else if (YAJL_IS_FALSE(iscommon)){
            printf("False\n");
            label = gtk_label_new("");
        }
        else {
            printf("Error: expected bool, but is_common was not bool\n");
            exit(1);
        }
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
        gtk_table_attach_defaults(GTK_TABLE(entries), label, 2, 3, i, i+1);
        gtk_widget_show(label);
    } 
    yajl_tree_free(root_node);
}
