#include "datastructure_definitions.h"

PadArea *pad_area_create ();
void pad_area_clear (PadArea *area);
void pad_area_undo_stroke (PadArea *area);
void pad_area_set_annotate (PadArea *area, gint annotate);;
void pad_area_set_auto_look_up (PadArea *area, gint auto_look_up);

void pad_area_changed_callback (PadArea *area);
