#ifndef KANJIPAD_ENGINE_RELATED_FUNCTIONS_HEADER
#define KANJIPAD_ENGINE_RELATED_FUNCTIONS_HEADER

#include <gtk/gtk.h>

gboolean engine_input_handler(GIOChannel* source, GIOCondition contidion, gpointer data);
void init_engine();

#endif /* KANJIPAD_ENGINE_RELATED_FUNCTIONS_HEADER */
