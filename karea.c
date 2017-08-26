#include "karea.h"
#include "global_vars.h"

void karea_get_char_size(GtkWidget* widget, int* width, int* height) {
    PangoLayout *layout = gtk_widget_create_pango_layout (widget, "\xe6\xb6\x88");
    pango_layout_get_pixel_size (layout, width, height);

    g_object_unref (layout);
}

gchar* utf8_for_char(kp_wchar ch) {
    gchar *string_utf;
    GError *err = NULL;
    gchar str[3];

    str[0] = ch.d[0] + 0x80;
    str[1] = ch.d[1] + 0x80;
    str[2] = 0;

    string_utf = g_convert (str, -1, "UTF-8", "EUC-JP", NULL, NULL, &err);
    if (!string_utf) {
        g_printerr ("Cannot convert string from EUC-JP to UTF-8: %s\n",
              err->message);
        exit (1);
    }

    return string_utf;
}

void karea_draw_character(GtkWidget* w, int index, int selected) {
    PangoLayout *layout;
    gchar *string_utf;
    gint char_width, char_height;
    gint x;

    karea_get_char_size (w, &char_width, &char_height);

    if (selected >= 0) {
        gdk_draw_rectangle (kpixmap,
              selected ? gtk_widget_get_style(w)->bg_gc[GTK_STATE_SELECTED] :
              gtk_widget_get_style(w)->white_gc,
              TRUE,
              0, (char_height + 6) *index, w->allocation.width - 1, char_height + 5);

    }

    string_utf = utf8_for_char (kanjiguess[index]);
    layout = gtk_widget_create_pango_layout (w, string_utf);
    g_free (string_utf);

    GtkAllocation *allocation = g_new0 (GtkAllocation, 1);
    gtk_widget_get_allocation(GTK_WIDGET(w), allocation);

    x = (allocation->width - char_width) / 2;

    gdk_draw_layout (kpixmap, 
           (selected > 0) ? gtk_widget_get_style(w)->white_gc :
                            gtk_widget_get_style(w)->black_gc,
           x, (char_height + 6) * index + 3, layout);

    g_object_unref (layout);
    g_free (allocation);   
}

void karea_draw(GtkWidget *w) {
    GtkAllocation *allocation = g_new0 (GtkAllocation, 1);
    gtk_widget_get_allocation(GTK_WIDGET(w), allocation);

    gint width = allocation->width;
    gint height = allocation->height;

    g_free (allocation);

    int i;

    gdk_draw_rectangle (kpixmap, 
              gtk_widget_get_style(w)->white_gc, TRUE,
              1, 1, width, height);


    for (i=0; i<num_guesses; i++)
    {
        if (WCHAR_EQ (kselected, kanjiguess[i]))
            karea_draw_character (w, i, 1);
        else
            karea_draw_character (w, i, -1);
    }

    gtk_widget_queue_draw (w);
}

int karea_configure_event(GtkWidget *w, GdkEventConfigure *event) {
    if (kpixmap)
        g_object_unref (kpixmap);

    kpixmap = gdk_pixmap_new (gtk_widget_get_window(w), event->width, event->height, -1);

    karea_draw (w);
  
    return TRUE;
}

int karea_expose_event(GtkWidget *w, GdkEventExpose *event) {
    if (!kpixmap)
        return 0;
  
    gdk_draw_drawable (gtk_widget_get_window(w),
		     gtk_widget_get_style(w)->fg_gc[GTK_STATE_NORMAL], kpixmap,
		     event->area.x, event->area.y,
		     event->area.x, event->area.y,
		     event->area.width, event->area.height);
  
    return 0;
}

int karea_erase_selection(GtkWidget* w) {
    int i;
    if (kselected.d[0] || kselected.d[1]) {
        for (i=0; i<num_guesses; i++) {
            if (WCHAR_EQ (kselected, kanjiguess[i])) {
                karea_draw_character (w, i, 0);
            }
        }
    }
    return TRUE;
}

void karea_primary_clear(GtkClipboard* clipboard, gpointer owner) {
    GtkWidget *w = owner;
  
    karea_erase_selection (w);
    kselected.d[0] = kselected.d[1] = 0;

    update_sensitivity ();
    gtk_widget_queue_draw (w);
}

void karea_primary_get(GtkClipboard* clipboard, GtkSelectionData* selection_data,
                       guint info, gpointer owner) {
    if (kselected.d[0] || kselected.d[1]) {
        gchar *string_utf = utf8_for_char (kselected);
        gtk_selection_data_set_text (selection_data, string_utf, -1);
        g_free (string_utf);
    }
}

int karea_button_press_event(GtkWidget* w, GdkEventButton* event) {
    int j;
    gint char_height;
    GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_PRIMARY);

    static const GtkTargetEntry targets[] = {
            { "STRING", 0, 0 },
            { "TEXT",   0, 0 }, 
            { "COMPOUND_TEXT", 0, 0 },
            { "UTF8_STRING", 0, 0 }
    };

    karea_erase_selection (w);

    karea_get_char_size (w, NULL, &char_height);

    j = event->y / (char_height + 6);
    if (j < num_guesses) {
        kselected = kanjiguess[j];
        karea_draw_character (w, j, 1);

        if (!gtk_clipboard_set_with_owner (clipboard, targets, G_N_ELEMENTS (targets),
                    karea_primary_get, karea_primary_clear, G_OBJECT (w)))
            karea_primary_clear (clipboard, w);
    }
    else {
        kselected.d[0] = 0;
        kselected.d[1] = 0;
        if (gtk_clipboard_get_owner (clipboard) == G_OBJECT (w))
            gtk_clipboard_clear (clipboard);
    }

    update_sensitivity ();
    gtk_widget_queue_draw (w);

    return TRUE;
}
