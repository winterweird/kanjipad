/* KanjiPad - Japanese handwriting recognition front end
 * Copyright (C) 1997 Owen Taylor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* NOTE: Further changes, enhancements and modifications have later been made
 * in accordance with this license by Vegard Itland (2017). I adopted this as a
 * personal project for personal use only, and as such I am making changes which
 * may reduce portability and/or usability for other purposes. The code was
 * cloned from the git repo https://github.com/boamaod/kanjipad.git at
 * 2017-08-25. I originally discovered the program at
 * http://fishsoup.net/software/kanjipad/ - however, I was unable to make it
 * compile, probably due to the usage of deprecated and/or obsolete GTK
 * constructs which my compiler didn't recognize.
 *
 * I take no credit for any of the work other than the additions and
 * modifications I have since written. I may refactor the code, but I will try
 * to properly mark each file with the original copyright notice. To see the
 * original project code, checkout commit 1097e82.
 *
 * Best regards,
 * Vegard Itland.
 */

// TODO: Query caching?

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "kanjipad.h"
#include "karea.h"       // added for refactoring purposes
#include "global_vars.h" // added for refactoring purposes
#include "callbacks.h"   // added for refactoring purposes
#include "engine.h"      // added for refactoring purposes

// NOTE: I left the following comments as is but I have no idea what it's all
// about... - Vegard
/* Wait for child process? */
  
/* Create Interface */

// print usage and exit
void usage () {
  fprintf(stderr, "Usage: %s [OPTIONS]\n", progname);
  fprintf(stderr, "Valid options:\n");
  fprintf(stderr, "   -f/--data-file FILE\n");
  fprintf(stderr, "       Specify an output file for testing purposes\n");
  fprintf(stderr, "   -w/--window-width INT\n");
  fprintf(stderr, "       Override the default window width (default = 350)\n");
  fprintf(stderr, "   -h/--window-height INT\n");
  fprintf(stderr, "       Override the default window height (default = 350)\n");
  fprintf(stderr, "   -r/--window-not-resizable\n");
  fprintf(stderr, "       Make window fixed-size (useful for small screens)\n");
  fprintf(stderr, "   -k/--keyboard-on-focus\n");
  fprintf(stderr, "       Display keyboard when entry box selected, hide when keyboard unfocused\n");
  exit (1);
}

// return the last element of the path to the program file
const char* getProgName(const char* p) {
    const char* progname = p;
    while (*p) {
        if (*p == '/') progname = p + 1;
        p++;
    }
    return progname;
}

// if data file is given, set data file name - otherwise if arguments given,
// print usage and exit
void handleArgs(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--data-file") == 0 || strcmp(argv[i], "-f") == 0) {
            i++;
            if (i < argc)
                data_file = argv[i];
            else
                usage();
        }
        else if (strcmp(argv[i], "--window-width") == 0 || strcmp(argv[i], "-w") == 0) {
            // set custom width
            i++;
            if (i < argc) {
                char* end;
                char* endOfArg = argv[i] + strlen(argv[i]);
                int argAsInt = (int)strtol(argv[i], &end, 10);
                if (end != endOfArg) {
                    usage();
                }
                else {
                    WINDOW_WIDTH = argAsInt;
                }
            }
            else {
                usage();
            }
        }
        else if (strcmp(argv[i], "--window-height") == 0 || strcmp(argv[i], "-h") == 0) {
            // set custom height
            i++;
            if (i < argc) {
                char* end;
                char* endOfArg = argv[i] + strlen(argv[i]);
                int argAsInt = (int)strtol(argv[i], &end, 10);
                if (end != endOfArg) {
                    usage();
                }
                else {
                    WINDOW_HEIGHT = argAsInt;
                }
            }
            else {
                usage();
            }
        }
        else if (strcmp(argv[i], "--keyboard-on-focus") == 0 || strcmp(argv[i], "-k") == 0) {
            // keyboard is shown when entry is selected and hidden once main
            // window regains focus
            KEYBOARD_ON_ENTRY_FOCUS = 1;
        }
        else if (strcmp(argv[i], "--window-not-resizable") == 0 || strcmp(argv[i], "-r") == 0) {
            // override resizability
            WINDOW_RESIZABLE = 0;
        }
        else {
            usage();
        }
    }
}

int main (int argc, char **argv) {
  GtkWidget *window;
  GtkWidget *main_hbox;
  GtkWidget *vseparator;
  GtkWidget *button;
  GtkWidget *main_vbox;
  GtkWidget *menubar;
  GtkWidget *vbox;
  GtkWidget *buttons_vbox;
  GtkWidget *label;
  
  GtkAccelGroup *accel_group;

  PangoFontDescription *font_desc;

  // I'm not sure why this needs to be a thing, but fine... - Vegard
  progname = getProgName(argv[0]);

  handleArgs(argc, argv);

  gtk_init (&argc, &argv);

  // window creation and setup
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_resizable (GTK_WINDOW (window), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (window), WINDOW_WIDTH, WINDOW_HEIGHT);

  if (!WINDOW_RESIZABLE) {
      // this makes the window more persistent in keeping its size
      gtk_widget_set_size_request (window, WINDOW_WIDTH, WINDOW_HEIGHT);
  }

  g_signal_connect (window, "destroy",
		    G_CALLBACK (exit_callback), NULL);
  g_signal_connect (window, "key_press_event",
          G_CALLBACK (handle_keypress_callback), NULL);

  gtk_window_set_title (GTK_WINDOW(window), "KanjiPad");
  // end window creation and setup
  
  main_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), main_vbox);
  gtk_widget_show (main_vbox);

  /* Menu */
    GtkActionGroup      *action_group;          /* Packing group for our Actions */
    GError              *error;                 /* For reporting exceptions or errors */
    GtkWidget           *toolbar;               /* The actual toolbar */
    
  action_group = gtk_action_group_new ("MainMenu");
  gtk_action_group_add_actions (action_group, entries, n_entries, NULL);
  gtk_action_group_add_toggle_actions (action_group, toggle_entries, n_toggle_entries, NULL);  
  
  menu_manager = gtk_ui_manager_new ();
  gtk_ui_manager_insert_action_group (menu_manager, action_group, 0);

  error = NULL;

  // Modification (reduces portability - assumes Linux OS): put ui.xml in
  // .kanjipad folder in home directory upon install and look there later when
  // you need it - allows kanjipad to be run from anywhere, not just from the
  // folder with ui.xml in it. - Vegard
  char* env = getenv("HOME");
  char* kanjipadData = "/.kanjipad/ui.xml";
  char* uiXmlPath = malloc(strlen(env) + strlen(kanjipadData) + 1);
  strcpy(uiXmlPath, env);
  strcat(uiXmlPath, "/.kanjipad/ui.xml");

  gtk_ui_manager_add_ui_from_file (menu_manager, uiXmlPath, &error);

  free(uiXmlPath);

  if (error){
        g_message ("building menus failed: %s", error->message);
        g_error_free (error);
  }

  //Add the menu bar
  menubar = gtk_ui_manager_get_widget (menu_manager, "/MainMenu");
  gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, FALSE, 0);

  
  /*accel_group = gtk_accel_group_new ();
  factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
  gtk_item_factory_create_items (factory, nmenu_items, menu_items, NULL);*/

  /* create a menubar */
  /*menubar = gtk_item_factory_get_widget (factory, "<main>");
  gtk_box_pack_start (GTK_BOX (main_vbox), menubar,
		      FALSE, TRUE, 0);
		      gtk_widget_show (menubar);*/

  /*  Install the accelerator table in the main window  */
  //gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

  main_hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX(main_vbox), main_hbox, TRUE, TRUE, 0);
  gtk_widget_show (main_hbox);


  // Create a new section below the rest of the interface which shows the
  // current jukugo constructed - Vegard
  GtkWidget* hseparator = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(main_vbox), hseparator, FALSE, FALSE, 0);
  gtk_widget_show(hseparator);

  GtkWidget* entryHbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), entryHbox, FALSE, FALSE, 0);
  gtk_widget_show(entryHbox);

  jukugo_entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(entryHbox), jukugo_entry, TRUE, TRUE, 0);
  gtk_widget_show(jukugo_entry);

  g_signal_connect(jukugo_entry, "key_press_event",
          G_CALLBACK(jukugo_keypress_callback), NULL);
  jukugo_entry_insert_text_handler_id = g_signal_connect_after(jukugo_entry, "insert-text",
          G_CALLBACK(replace_hiragana_callback), NULL);
  if (KEYBOARD_ON_ENTRY_FOCUS) {
      g_signal_connect(jukugo_entry, "button-press-event",
              G_CALLBACK(show_keyboard_callback), NULL);
  }

  // another hseparator
  hseparator = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(main_vbox), hseparator, FALSE, FALSE, 0);
  gtk_widget_show(hseparator);

  // add scroll window for results display
  
  scrollingResults = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollingResults),
          GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  // NOTE: min/max apparently doesn't exist in standard C according to one
  // random stackoverflow post I came across. I might implement it later.
  int spaceWeNeed = WINDOW_HEIGHT - 65; // calculated by trial and error
  if (WINDOW_RESIZABLE || RESULTS_SCROLL_WINDOW_SIZE < spaceWeNeed)
      gtk_widget_set_size_request(scrollingResults, 0, RESULTS_SCROLL_WINDOW_SIZE);
  else
      // we want a bit of margin, because if it fills the entire window we can't
      // really remove it .-. BUGS AND WORKAROUNDS
      gtk_widget_set_size_request(scrollingResults, 0, spaceWeNeed);

  gtk_box_pack_start(GTK_BOX(main_vbox), scrollingResults, FALSE, FALSE, 0);
  gtk_widget_hide(scrollingResults); // for now

  // add box for adding information from jisho.org
  results_display_box = gtk_vbox_new(FALSE, 0);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrollingResults), results_display_box);
  gtk_widget_show(results_display_box);


  // note: none of this is actually really displaying, I'm not sure why
//  label = gtk_label_new("Search Jisho.org");
//  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
////  gtk_widget_modify_font (label, font_desc); // for some reason this crashes the program
//  gtk_widget_show(label);
//
//  GtkWidget* jukugo_search_button = gtk_button_new();
//  gtk_container_add(GTK_CONTAINER(jukugo_search_button), label);
//  g_signal_connect(jukugo_search_button, "clicked",
//          G_CALLBACK(jisho_search_callback), NULL);
//  gtk_widget_show(jukugo_search_button);

  /*  toolbar = gtk_ui_manager_get_widget (menu_manager, "/MainToolbar");
  gtk_box_pack_start (GTK_BOX (main_hbox), toolbar, FALSE, FALSE, 0);
  gtk_window_add_accel_group (GTK_WINDOW (window), gtk_ui_manager_get_accel_group (menu_manager));
  */
    
  /* Area for user to draw characters in */

  pad_area = pad_area_create ();

  gtk_box_pack_start (GTK_BOX (main_hbox), pad_area->widget, TRUE, TRUE, 0);
  gtk_widget_show (pad_area->widget);

  vseparator = gtk_vseparator_new();
  gtk_box_pack_start (GTK_BOX (main_hbox), vseparator, FALSE, FALSE, 0);
  gtk_widget_show (vseparator);

  // put the vbox for the buttons in the window
  buttons_vbox = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(main_hbox), buttons_vbox, FALSE, FALSE, 0);
  gtk_widget_show(buttons_vbox);


  /* Area in which to draw guesses */

  GtkWidget* scrollingSuggestions = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollingSuggestions),
          GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_box_pack_start(GTK_BOX(main_hbox), scrollingSuggestions, FALSE, FALSE, 0);
  gtk_widget_show(scrollingSuggestions);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrollingSuggestions), vbox);
  gtk_widget_show (vbox);

  karea = gtk_drawing_area_new();

  g_signal_connect (karea, "configure_event",
		    G_CALLBACK (karea_configure_event), NULL);
  g_signal_connect (karea, "expose_event",
		    G_CALLBACK (karea_expose_event), NULL);
  g_signal_connect (karea, "button_press_event",
		    G_CALLBACK (karea_button_press_event), NULL);

  gtk_widget_set_events (karea, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

#ifdef G_OS_WIN32
  font_desc = pango_font_description_from_string ("MS Gothic 18");
#else
  font_desc = pango_font_description_from_string ("Sans 18");
#endif  
  gtk_widget_modify_font (karea, font_desc);
  
  gtk_box_pack_start (GTK_BOX (vbox), karea, TRUE, TRUE, 0);
  gtk_widget_show (karea);


  /* Buttons */
  label = gtk_label_new ("\xe5\xbc\x95");
  /* We have to set the alignment here, since GTK+ will fail
   * to get the width of the string appropriately...
   */
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_modify_font (label, font_desc);
  gtk_widget_show (label);
  
  lookup_button = button = gtk_button_new ();
  gtk_container_add (GTK_CONTAINER (button), label);
  g_signal_connect (button, "clicked",
		    G_CALLBACK (look_up_callback), NULL);

  gtk_box_pack_start (GTK_BOX (buttons_vbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  label = gtk_label_new ("\xe6\x88\xbb");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_modify_font (label, font_desc);
  gtk_widget_show (label);

  undo_button = button = gtk_button_new ();
  gtk_container_add (GTK_CONTAINER (button), label);
  g_signal_connect (button, "clicked",
		    G_CALLBACK (undo_callback), NULL);

  gtk_box_pack_start (GTK_BOX (buttons_vbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);
  
  label = gtk_label_new ("\xe6\xb6\x88");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_modify_font (label, font_desc);
  gtk_widget_show (label);

  clear_button = button = gtk_button_new ();
  gtk_container_add (GTK_CONTAINER (button), label);
  g_signal_connect (button, "clicked",
		    G_CALLBACK (clear_callback), NULL);

  gtk_box_pack_start (GTK_BOX (buttons_vbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  // make a label for clearing input from search box
  label = gtk_label_new("\xe2\x9c\x96"); // thick multiplication x
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
  gtk_widget_modify_font(label, font_desc);
  gtk_widget_show(label);
  
  // make a button to clear input from search box with
  GtkWidget* clearSearchFieldButton = button = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(button), label);
  g_signal_connect(button, "clicked",
          G_CALLBACK(clear_search_field), NULL);

  gtk_box_pack_start (GTK_BOX (entryHbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  // make a label for the jisho search button
  label = gtk_label_new("\xe2\x9e\xa4"); // black right arrowhead
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
  gtk_widget_modify_font(label, font_desc);
  gtk_widget_show(label);

  // make a button to search jisho with
  GtkWidget* searchJishoButton = button = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(button), label);
  g_signal_connect(button, "clicked",
          G_CALLBACK(query_jisho), NULL);
    
  gtk_box_pack_start (GTK_BOX (entryHbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  gtk_widget_show(window);

  pango_font_description_free (font_desc);

  init_engine();

  update_sensitivity ();

  gtk_main();

  return 0;
}
