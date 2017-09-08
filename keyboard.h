#ifndef KANJIPAD_VIRTUAL_KEYBOARD_FUNCTIONS_HEADER
#define KANJIPAD_VIRTUAL_KEYBOARD_FUNCTIONS_HEADER

#include <gtk/gtk.h> // gboolean

void keyboard_set_visible(gboolean isVisible);
void keyboard_init_display();
void keyboard_reposition();

#endif /* KANJIPAD_VIRTUAL_KEYBOARD_FUNCTIONS_HEADER */
