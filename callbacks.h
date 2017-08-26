#ifndef KANJIPAD_CALLBACK_FUNCTIONS_HEADER
#define KANJIPAD_CALLBACK_FUNCTIONS_HEADER

#include <gtk/gtkcheckmenuitem.h>
//#include <gdk/gdkkeysyms.h>  // event codes
//#include <gdk/gdk.h>
//#include <gtk/gtk.h>

void exit_callback();
void undo_callback();
void copy_callback();
void save_callback();
void clear_callback();
void look_up_callback();
void annotate_callback(GtkCheckMenuItem *menu_item, gpointer user_data);
void auto_look_up_callback(GtkCheckMenuItem *menu_item, gpointer user_data);

// function which handles keypresses (bound to window)
gboolean handle_keypress_callback(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

#endif /* KANJIPAD_CALLBACK_FUNCTIONS_HEADER */
