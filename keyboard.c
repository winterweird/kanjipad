#include "keyboard.h"
#include "global_vars.h"
#include "callbacks.h"

void keyboard_set_visible(gboolean isVisible) {
    if (isVisible != keyboard.isVisible) {
        keyboard.isVisible = isVisible;
        keyboard_init_display();
    }
}

static void addbutton(GtkWidget* box, const char* l, GCallback func) {
    GtkWidget *button, *label;
    
    label = gtk_label_new(l);
    gtk_widget_show(label);
    
    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button), label);
    gtk_widget_set_size_request(button, KBD_BUTTON_SIZE, KBD_BUTTON_SIZE);
    if (func == NULL) {
        // make new callback func
        func = G_CALLBACK(insert_char_callback);
    }
    // This looks like the best option, since I'm passing dynamically allocated
    // strings. I'm using a long to store the value to quell warnings from the
    // compiler, then casting to a char in the callback function and using it to
    // reconstruct a string which is then inserted in the appropriate entry
    // widget.
    long ch = *l;
    g_signal_connect(button, "clicked", func, (void*)(ch));
    gtk_widget_show(button);

    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
}

static void populaterow(GtkWidget* row, const char* characters) {
    for (const char* ch = characters; *ch != 0; ch++) {
        char letter[2] = {0};
        letter[0] = *ch;
        addbutton(row, letter, NULL);
    }
}

void keyboard_reposition() {
    gint wx, wy, x, y, yoffset;

    GtkWidget* toplevel = gtk_widget_get_toplevel(jukugo_entry);
    GdkWindow* gdkToplevel = gtk_widget_get_window(toplevel);
    gdk_window_get_position(gdkToplevel, &wx, &wy);
    
    gtk_widget_get_size_request(keyboard.kbdWidget, NULL, &yoffset);

    gtk_widget_translate_coordinates(jukugo_entry, toplevel, 0, 0, &x, &y);
    gtk_window_move(GTK_WINDOW(keyboard.kbdWidget), wx + x, wy + y - yoffset);
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
        GtkWidget *label, *button; // used for adding buttons with more complex actions

        // top row
        row = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(kbdvbox), row, FALSE, FALSE, 0);
        gtk_widget_show(row);
        populaterow(row, "qwertyuiop");

        // make backspace button
        label = gtk_label_new("\xf0\x9f\xa1\xb8"); // wide-headed leftwards heavy barb arrow
        gtk_widget_show(label);

        button = gtk_button_new();
        gtk_container_add(GTK_CONTAINER(button), label);
        gtk_widget_set_size_request(button, KBD_BUTTON_SIZE, KBD_BUTTON_SIZE);
        g_signal_connect(button, "clicked", 
                G_CALLBACK(delete_char_callback), NULL);
        gtk_widget_show(button);
        gtk_box_pack_start(GTK_BOX(row), button, FALSE, FALSE, 0);
        
        // home row
        row = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(kbdvbox), row, FALSE, FALSE, 0);
        populaterow(row, "asdfghjkl\"");
        gtk_widget_show(row);

        // search button
        label = gtk_label_new("\xf0\x9f\x94\x8e"); // right-pointing magnifying glass
        gtk_widget_show(label);

        button = gtk_button_new();
        gtk_container_add(GTK_CONTAINER(button), label);
        gtk_widget_set_size_request(button, KBD_BUTTON_SIZE, KBD_BUTTON_SIZE);
        g_signal_connect(button, "clicked", 
                G_CALLBACK(query_jisho), NULL);
        gtk_widget_show(button);
        gtk_box_pack_start(GTK_BOX(row), button, FALSE, FALSE, 0);
        
        // bottom row
        row = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(kbdvbox), row, FALSE, FALSE, 0);
        populaterow(row, "zxcvbnm*?");
        gtk_widget_show(row);
        
        // left arrow button
        label = gtk_label_new("\xe2\x97\x80"); // black left-pointing triangle
        gtk_widget_show(label);

        button = gtk_button_new();
        gtk_container_add(GTK_CONTAINER(button), label);
        gtk_widget_set_size_request(button, KBD_BUTTON_SIZE, KBD_BUTTON_SIZE);
        g_signal_connect(button, "clicked", 
                G_CALLBACK(cursor_pos_left_callback), NULL);
        gtk_widget_show(button);
        gtk_box_pack_start(GTK_BOX(row), button, FALSE, FALSE, 0);

        // right arrow button
        label = gtk_label_new("\xe2\x96\xb6"); // black right-pointing triangle
        gtk_widget_show(label);

        button = gtk_button_new();
        gtk_container_add(GTK_CONTAINER(button), label);
        gtk_widget_set_size_request(button, KBD_BUTTON_SIZE, KBD_BUTTON_SIZE);
        g_signal_connect(button, "clicked", 
                G_CALLBACK(cursor_pos_right_callback), NULL);
        gtk_widget_show(button);
        gtk_box_pack_start(GTK_BOX(row), button, FALSE, FALSE, 0);
        
        // when karea (kanji draw area) needs to be redrawn for some reason,
        // reposition the keyboard (this is likely an indication that the
        // results box was shown or hidden)
        g_signal_connect(karea, "expose-event",
                G_CALLBACK(keyboard_reposition), NULL);
        
        keyboard.isSetup = TRUE;
    }

    if (keyboard.isVisible) {
        keyboard_reposition();
        gtk_widget_show(keyboard.kbdWidget);
    }
    else {
        gtk_widget_hide(keyboard.kbdWidget);
    }
}
