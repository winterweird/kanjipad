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

/** Refactored and modified by Vegard Itland (2017) 
 *  
 *  NOTE: This file was here before I got my hands on the project, but I have
 *  indeed modified it. The copyright notice wasn't here before, I think, but I
 *  added it in anyways for good measure.
 *
 *  Best regards,
 *  Vegard.
 */

#include "datastructure_definitions.h"

PadArea *pad_area_create ();
void pad_area_clear (PadArea *area);
void pad_area_undo_stroke (PadArea *area);
void pad_area_set_annotate (PadArea *area, gint annotate);;
void pad_area_set_auto_look_up (PadArea *area, gint auto_look_up);

void pad_area_changed_callback (PadArea *area);
