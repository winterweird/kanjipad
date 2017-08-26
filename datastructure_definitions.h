#ifndef KANJIPAD_DATASTRUCTURE_DEFINITIONS_HEADER
#define KANJIPAD_DATASTRUCTURE_DEFINITIONS_HEADER

#include <gtk/gtk.h>

typedef struct _PadArea PadArea;

struct _PadArea {
  GtkWidget *widget;

  gint annotate;
  gint auto_look_up;
  GList *strokes;

  /* Private */
  GdkPixmap *pixmap;
  GList *curstroke;
  int instroke;
};

typedef struct {
  gchar d[2];
} kp_wchar;

#endif /* KANJIPAD_DATASTRUCTURE_DEFINITIONS_HEADER */
