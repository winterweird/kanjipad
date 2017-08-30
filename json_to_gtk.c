#include <stdio.h>
#include "json_to_gtk.h"
#include "yajl/yajl_tree.h"

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
}
