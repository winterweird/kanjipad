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

typedef struct _Keyboard {
    gboolean isVisible;
    GtkWidget* kbdWidget;
} Keyboard;

#endif /* KANJIPAD_DATASTRUCTURE_DEFINITIONS_HEADER */
