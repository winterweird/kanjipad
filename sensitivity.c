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

/** Refactored and modified by Vegard Itland (2017) **/

#include "sensitivity.h"
#include "global_vars.h"

void update_path_sensitive (const gchar *path, gboolean sensitive) {
  /*TODO GtkWidget *widget = gtk_item_factory_get_widget (factory, path);
    gtk_widget_set_sensitive (widget, sensitive);*/
}


void update_sensitivity () {
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
