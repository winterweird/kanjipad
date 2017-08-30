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
 *  the RESTful API of jisho.org, an online Japanese dictionary. In order to
 *  parse the JSON results returned from jisho.org, I am utilizing a JSON
 *  validating/parsing library called "YAJL" (Yet Another JSON Library). Much
 *  thanks to the creator! Check it out at http://lloyd.github.io/yajl/
 */

// NOTE: Am I rad at naming shit or what?
#ifndef KANJIPAD_JSON_PARSING_ETC_FUNCTIONS_HEADER
#define KANJIPAD_JSON_PARSING_ETC_FUNCTIONS_HEADER

// Note: signature might change if I feel called to it.
void display_results(const char* results);

#endif /* KANJIPAD_JSON_PARSING_ETC_FUNCTIONS_HEADER */
