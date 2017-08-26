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
