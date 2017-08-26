#include "callbacks.h"
#include "global_vars.h"
#include <errno.h>

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
