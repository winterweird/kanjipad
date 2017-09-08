#include "keyboard.h"
#include "global_vars.h"

void keyboard_set_visible(gboolean isVisible) {
    if (isVisible != keyboard.isVisible) {
        keyboard.isVisible = isVisible;
        // TODO: more stuff
    }
}
