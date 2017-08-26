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
 * to properly mark each file with the original copyright notice. Code may be
 * assumed to be written by the original author unless otherwise noted.
 *
 * Best regards,
 * Vegard Itland.
 */

#include <gtk/gtk.h>
//#include <gtk/gtkcheckmenuitem.h> // removed due to refactoring
#include <gdk/gdkkeysyms.h>  // added for keyboard interaction purposes
//#include <ctype.h>  // removed due to refactoring
//#include <errno.h>  // removed due to refactoring
#include <stdlib.h>
//#include <stdio.h>  // removed due to refactoring
#include <string.h>
//#include <unistd.h> // removed due to refactoring

#include "kanjipad.h"
#include "karea.h"       // added for refactoring purposes
#include "global_vars.h" // added for refactoring purposes
#include "callbacks.h"   // added for refactoring purposes
#include "engine.h"      // added for refactoring purposes

// NOTE: I left the following comment as is but I have no idea what it's all
// about... - Vegard
/* Wait for child process? */

void update_sensitivity ();

// TODO: refactor or remove
static void
jisho_search_callback(GtkWidget *w) {
    // TODO: figure out, I can't be arsed tonight.
}

void
pad_area_changed_callback (PadArea *area)
{
  update_sensitivity ();
  if(area->auto_look_up)
    {
      look_up_callback (NULL);
    }
}

static void
update_path_sensitive (const gchar *path,
		       gboolean     sensitive)
{
  /*TODO GtkWidget *widget = gtk_item_factory_get_widget (factory, path);
    gtk_widget_set_sensitive (widget, sensitive);*/
}

void
update_sensitivity ()
{
  gboolean have_selected = (kselected.d[0] || kselected.d[1]);
  gboolean have_strokes = (pad_area->strokes != NULL);

  update_path_sensitive ("/Edit/Undo", have_strokes);
  update_path_sensitive ("/Edit/Copy", have_selected);
  update_path_sensitive ("/Edit/Clear", have_strokes);
  update_path_sensitive ("/Character/Lookup", have_strokes);
  gtk_widget_set_sensitive (lookup_button, have_strokes);
  update_path_sensitive ("/Character/Clear", have_strokes);
  gtk_widget_set_sensitive (clear_button, have_strokes);
  gtk_widget_set_sensitive (undo_button, have_strokes);
  update_path_sensitive ("/Character/Save", have_strokes);
}

static gboolean
handle_keypress_callback(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    if (event->state & GDK_CONTROL_MASK) {
        switch (event->keyval) {
            case GDK_q:
                exit_callback(widget);
                break;
            case GDK_c:
                copy_callback(widget);
                break;
            case GDK_x:
                clear_callback(widget);
                break;
            case GDK_z:
                undo_callback(widget);
                break;
            case GDK_l:
                look_up_callback(widget);
                break;
            case GDK_s:
                save_callback(widget);
                break;
            default:
                return FALSE;
        }
    }
    return FALSE;
}
  
/* Create Interface */

void
usage ()
{
  fprintf(stderr, "Usage: %s [-f/--data-file FILE]\n", progname);
  exit (1);
}

int 
main (int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *main_hbox;
  GtkWidget *vseparator;
  GtkWidget *button;
  GtkWidget *main_vbox;
  GtkWidget *menubar;
  GtkWidget *vbox;
  GtkWidget *label;
  
  GtkAccelGroup *accel_group;

  PangoFontDescription *font_desc;
  int i;
  char *p;

  p = progname = argv[0];
  while (*p)
    {
      if (*p == '/') progname = p+1;
      p++;
    }

  gtk_init (&argc, &argv);

  for (i=1; i<argc; i++)
    {
      if (!strcmp(argv[i], "--data-file") ||
	  !strcmp(argv[i], "-f"))
	{
	  i++;
	  if (i < argc)
	    data_file = argv[i];
	  else
	    usage();
	}
      else
	{
	  usage();
	}
    }

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_resizable (GTK_WINDOW (window), TRUE);
  gtk_window_set_default_size (GTK_WINDOW (window), 350, 350);

  g_signal_connect (window, "destroy",
		    G_CALLBACK (exit_callback), NULL);
  g_signal_connect (window, "key_press_event",
          G_CALLBACK (handle_keypress_callback), NULL);

  gtk_window_set_title (GTK_WINDOW(window), "KanjiPad");
  
  main_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), main_vbox);
  gtk_widget_show (main_vbox);

  /* Menu */
    GtkActionGroup      *action_group;          /* Packing group for our Actions */
    GtkUIManager        *menu_manager;          /* The magic widget! */
    GError              *error;                 /* For reporting exceptions or errors */
    GtkWidget           *toolbar;               /* The actual toolbar */
    
  action_group = gtk_action_group_new ("MainMenu");
  gtk_action_group_add_actions (action_group, entries, n_entries, NULL);
  gtk_action_group_add_toggle_actions (action_group, toggle_entries, n_toggle_entries, NULL);  
  
  menu_manager = gtk_ui_manager_new ();
  gtk_ui_manager_insert_action_group (menu_manager, action_group, 0);

  error = NULL;
  /** This is just my fuckery, tbh, but fuck it **/
  char* env = getenv("HOME");                                         // fuckery
  char* kanjipadData = "/.kanjipad/ui.xml";                           // fuckery
  char* uiXmlPath = malloc(strlen(env) + strlen(kanjipadData) + 1);   // fuckery
  strcpy(uiXmlPath, env);                                             // fuckery
  strcat(uiXmlPath, "/.kanjipad/ui.xml");                             // fuckery

  gtk_ui_manager_add_ui_from_file (menu_manager, uiXmlPath, &error);  // a little bit fuckery

  if (error){
        g_message ("building menus failed: %s", error->message);
        g_error_free (error);
  }
  
  free(uiXmlPath);                                                    // fuckery
  /** End of fuckery **/

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
  gtk_container_add (GTK_CONTAINER(window), main_hbox);
  gtk_box_pack_start (GTK_BOX(main_vbox), main_hbox, TRUE, TRUE, 0);
  gtk_widget_show (main_hbox);


  // more fuckery
  GtkWidget* hseparator = gtk_hseparator_new(); // fuckery
  gtk_box_pack_start(GTK_BOX(main_vbox), hseparator, FALSE, FALSE, 0); // fuckery
  gtk_widget_show(hseparator); // fuckery

  GtkWidget* jukugo_entry = gtk_entry_new(); // fuckery
  gtk_container_add(GTK_CONTAINER(window), jukugo_entry); // fuckery
  gtk_box_pack_start(GTK_BOX(main_vbox), jukugo_entry, TRUE, TRUE, 0); // fuckery
  gtk_widget_show(jukugo_entry); // fuckery

  // note: none of this is actually really displaying, I'm not sure why
  /** FUCKERY **/
  label = gtk_label_new("Search Jisho.org");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
//  gtk_widget_modify_font (label, font_desc); // for some reason this crashes the program
  gtk_widget_show(label);

  GtkWidget* jukugo_search_button = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(jukugo_search_button), label);
  g_signal_connect(jukugo_search_button, "clicked",
          G_CALLBACK(jisho_search_callback), NULL);
  gtk_widget_show(jukugo_search_button);
  /** FUCKERY **/

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
  
  /* Area in which to draw guesses */

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (main_hbox), vbox, FALSE, FALSE, 0);
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

  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  label = gtk_label_new ("\xe6\x88\xbb");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_modify_font (label, font_desc);
  gtk_widget_show (label);

  undo_button = button = gtk_button_new ();
  gtk_container_add (GTK_CONTAINER (button), label);
  g_signal_connect (button, "clicked",
		    G_CALLBACK (undo_callback), NULL);

  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);
  
  label = gtk_label_new ("\xe6\xb6\x88");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_modify_font (label, font_desc);
  gtk_widget_show (label);

  clear_button = button = gtk_button_new ();
  gtk_container_add (GTK_CONTAINER (button), label);
  g_signal_connect (button, "clicked",
		    G_CALLBACK (clear_callback), NULL);

  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  gtk_widget_show(window);

  pango_font_description_free (font_desc);

  init_engine();

  update_sensitivity ();

  gtk_main();

  return 0;
}
