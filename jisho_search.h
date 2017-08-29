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
 *  NOTE: This is an original feature I (Vegard) am developing. It's based on
 *  the RESTful API of jisho.org, an online Japanese dictionary. Much thanks to
 *  the creator of the jisho.org website and the Stack Overflow user whose code
 *  example was a tremendous help in getting HTTP GET requests working in C.
 */

#ifndef KANJIPAD_JISHO_SEARCH_INTERFACE_HEADER
#define KANJIPAD_JISHO_SEARCH_INTERFACE_HEADER

// returns dynamically allocated memory; call free after use
char* jisho_search_keyword(const char* keyword);

#endif /* KANJIPAD_JISHO_SEARCH_INTERFACE_HEADER */
