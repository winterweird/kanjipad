#include "keyboard.h"
#include "global_vars.h"

void keyboard_set_visible(gboolean isVisible) {
    if (isVisible != keyboard.isVisible) {
        keyboard.isVisible = isVisible;
        keyboard_init_display();
    }
}

static void addbutton(GtkWidget* box, const char* l) {
    GtkWidget *button, *label;
    
    label = gtk_label_new(l);
    gtk_widget_show(label);
    
    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button), label);
    gtk_widget_set_size_request(button, KBD_BUTTON_SIZE, KBD_BUTTON_SIZE);
    gtk_widget_show(button);

    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
}

static void populaterow(GtkWidget* row, const char* characters) {
    for (const char* ch = characters; *ch != 0; ch++) {
        char letter[2] = {0};
        letter[0] = *ch;
        addbutton(row, letter);
    }
}

void keyboard_init_display() {
    if (!keyboard.isSetup) {
        keyboard.kbdWidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_widget_set_size_request(keyboard.kbdWidget, WINDOW_WIDTH, 3*KBD_BUTTON_SIZE);
        gtk_window_set_decorated(GTK_WINDOW(keyboard.kbdWidget), FALSE);
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(keyboard.kbdWidget), TRUE);

        // I just picked a global widget, albeit one which is related to this
        // window
        GtkWidget* toplevel = gtk_widget_get_toplevel(jukugo_entry);
        
        // make keyboard always on top of main window
        gtk_window_set_transient_for(GTK_WINDOW(keyboard.kbdWidget), GTK_WINDOW(toplevel));

        // main vbox in keyboard
        GtkWidget* kbdvbox = gtk_vbox_new(FALSE, 0);
        gtk_container_add(GTK_CONTAINER(keyboard.kbdWidget), kbdvbox);
        gtk_widget_show(kbdvbox);

        GtkWidget* row; // used for adding new rows

        row = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(kbdvbox), row, FALSE, FALSE, 0);
        gtk_widget_show(row);
        populaterow(row, "qwertyuiop");
        
        row = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(kbdvbox), row, FALSE, FALSE, 0);
        populaterow(row, "asdfghjkl");
        gtk_widget_show(row);
        
        row = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(kbdvbox), row, FALSE, FALSE, 0);
        populaterow(row, "zxcvbnm");
        gtk_widget_show(row);
        
        gint wx, wy, x, y, yoffset;

        GdkWindow* gdkToplevel = gtk_widget_get_window(toplevel);
        gdk_window_get_position(gdkToplevel, &wx, &wy);
        
        gtk_widget_get_size_request(keyboard.kbdWidget, NULL, &yoffset);

        gtk_widget_translate_coordinates(jukugo_entry, toplevel, 0, 0, &x, &y);
        gtk_window_move(GTK_WINDOW(keyboard.kbdWidget), wx + x, wy + y - yoffset);
        
        keyboard.isSetup = TRUE;
    }

    if (keyboard.isVisible) {
        gtk_widget_show(keyboard.kbdWidget);
    }
    else {
        gtk_widget_hide(keyboard.kbdWidget);
    }
}
