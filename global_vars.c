#include "global_vars.h"
#include "callbacks.h"

cairo_surface_t* kpixmap;
GtkWidget *karea;
GtkWidget *clear_button;
GtkWidget *undo_button;
GtkWidget *lookup_button;
GtkUIManager *ui_manager;

kp_wchar kanjiguess[MAX_GUESSES];
int num_guesses = 0;
kp_wchar kselected;

PadArea *pad_area;

/* globals for engine communication */
int engine_pid;
GIOChannel *from_engine;
GIOChannel *to_engine;

char *data_file = NULL;
const char *progname;

GtkActionEntry entries[] = {

    { "FileMenuAction", NULL, "_File" },                  /* name, stock id, label */
    { "CharacterMenuAction", NULL, "_Character" },
    { "EditMenuAction", NULL, "_Edit" },

    //file  
    { "QuitAction", NULL,
      "_Quit", "<control>Q",    
      "Quit",
      G_CALLBACK (exit_callback) },

    //edit
    { "UndoAction", NULL,
      "_Undo", "<control>Z",  
      "undo",
      G_CALLBACK (undo_callback) },

    { "CopyAction", NULL,
      "_Copy", "<control>C",  
      "copy",
      G_CALLBACK (copy_callback) },

    //character
    { "LookupAction", NULL,   
      "_Lookup", "<control>L",          
      "Look-up the character drawn",    
      G_CALLBACK (look_up_callback) },
    
    { "ClearAction", NULL,
      "_Clear","<control>X",  
      "Clear the drawing area",
      G_CALLBACK (clear_callback) },

    { "SaveAction", NULL,
      "_Save","<control>S",  
      "write out the points in the character, \
and the selected character to a file \"samples.dat\" in the \
current directory. This is intended for making a file of \
characters for automated testing",
      G_CALLBACK (save_callback) }  
};

guint n_entries = G_N_ELEMENTS (entries);

GtkToggleActionEntry toggle_entries[] = {
    { "AnnotateAction", NULL,
      "_Annotate", NULL,  
      "Display stroke order",
      G_CALLBACK (annotate_callback), TRUE }, 

    { "AutoLookupAction", NULL,
      "_Auto Lookup", NULL,  
      "Lookup when a stroke is drawn",
      G_CALLBACK (auto_look_up_callback), TRUE }
};

guint n_toggle_entries = G_N_ELEMENTS (toggle_entries);
