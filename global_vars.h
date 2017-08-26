#ifndef KANJIPAD_GLOBAL_VARIABLES_HEADER
#define KANJIPAD_GLOBAL_VARIABLES_HEADER

#include "preprocessor_definitions.h"
#include "datastructure_definitions.h"

/* user interface elements */
//static GdkPixmap *kpixmap;
extern cairo_surface_t* kpixmap;
extern GtkWidget* karea;
extern GtkWidget* clear_button;
extern GtkWidget* undo_button;
extern GtkWidget* lookup_button;
extern GtkUIManager* ui_manager;

extern kp_wchar kanjiguess[MAX_GUESSES];
extern int num_guesses;
extern kp_wchar kselected;

extern PadArea* pad_area;

/* globals for engine communication */
extern int engine_pid;
extern GIOChannel* from_engine;
extern GIOChannel* to_engine;

extern char* data_file;
extern char* progname;

// 9 is hardcoded based on the count I made earlier, this might create problems.
extern GtkActionEntry entries[9];

extern guint n_entries;

// 2 is similarly hardcoded
extern GtkToggleActionEntry toggle_entries[2];

extern guint n_toggle_entries;

#endif /* KANJIPAD_GLOBAL_VARIABLES_HEADER */
