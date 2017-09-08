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

// NOTE: I'm not sure it makes sense to have a header like this, but screw it.

#ifndef KANJIPAD_PREPROCESSOR_DEFINITIONS_HEADER
#define KANJIPAD_PREPROCESSOR_DEFINITIONS_HEADER

#define WCHAR_EQ(a,b) (a.d[0] == b.d[0] && a.d[1] == b.d[1])

// used for defining the size of a global array variable
#define MAX_GUESSES 100

#define BUFLEN 256

#define RESULTS_SCROLL_WINDOW_SIZE 200

#define DEFAULT_WINDOW_WIDTH 350
#define DEFAULT_WINDOW_HEIGHT 350

// whether or not the keyboard is initially visible
#define KBD_INIT_VISIBLE FALSE

// virtual keyboard button size
#define KBD_BUTTON_SIZE 25

#endif /* KANJIPAD_PREPROCESSOR_DEFINITIONS_HEADER */
