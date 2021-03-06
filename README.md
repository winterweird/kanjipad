NOTE: This version of KanjiPad is an updated version forked by me, which
integrates querying jisho.org using its API into the program. Some features have
been added and some have been modified. The original version of the README -
updated to markdown format - can be found below.

I take no credit for any part of this project other than the modifications and
additions I have made. This project, even in its current form, has the potential
to be useful for me, and hopefully it might also be useful to someone else.

An important feature which has been added: a search box! Also, a scrolling
suggestion box. Double click a suggested kanji (or mark it and press Ctrl+A) in
order to add it to the search box. Click the arrow to query jisho.org or the
cross-marked button to clear the search box. Aside from that, the usage of this
program is probably quite intuitive, as long as you remember that the number of
strokes (and their order) matters.

I might add more notes about features and/or usage later on. Best regards! -
Vegard

# KanjiPad

KanjiPad is a very simple program for handwriting recognition -
The user draws a character into the box, then requests translation.
The best candidates are displayed along the right hand side of
the window and can be selected for pasting into other programs.

It is meant primarily for dictionary purposes for learners of Japanese
- it does not support entering kana, so it's usefulness as an input
method is limited. Furthermore, if you already know the reading of a
character, conventional pronunciation based methods of entering the
character are probably faster.

The handwriting recognition algorithms were invented by Todd David
Rudick for his program JavaDict:

  http://www.cs.arizona.edu/japan/JavaDict/

and translated into C by Robert Wells for his program JStroke:

  http://www.wellscs.com/pilot/

JStroke runs on Pilot handheld computers. The contents of the jstroke/
subdirectory are from the JStroke distribution.

KanjiPad works fine with a mouse. However, drawing characters
is faster, more accurate, and more fun with a graphics tablet.
(I use the Wacom Artpad II, available for about US $110.00) For
more information about using graphics tablets with X Windows,
see the XInput HOWTO at:

  http://www.gtk.org/~otaylor/xinput/XInput-HOWTO.html

## Availability

The latest version of KanjiPad is available from 

  http://fishsoup.net/software/kanjipad/

## Prerequisites

KanjiPad uses the GTK+ toolkit, version 2.0; it's been mostly developed
on Unix and Linux, but with version 2.0 of GTK+, it can also be
used on Microsoft Windows. GTK is available from:

  ftp://ftp.gtk.org/

Information about GTK+ on Win32 is available at:

  http://www.gimp.org/~tml/gimp/win32/

Note that you'll need the GTK+-2.0 packages, not the GTK+-1.3.0
version.

Perl is also required to build the file 'jdata.dat'. Information
about perl is available from 

  http://www.perl.com

GNU make is required for building KanjiPad. You probably already have
this installed, but if not, you can get it from:

  http://www.gnu.org/software/make/


## Installation

* Make sure that pkg-config is in your path, and if you installed
  GTK+ in a different prefix than pkg-config, that PKG_CONFIG_PATH
  includes /prefix/of/gtk/lib/pkgconfig.

* Edit the makefile and check that BINDIR and LIBDIR are set to the 
  location where you want to install KanjiPad.

* Build the program
   make
  [ Become root if necessary ]
   make install

## Use

Draw your character into the large box, and click on "引"
(The upper button, abbreviating "look up")

The use of Japanese abbreviations is for space reasons. I
may eventually replace them with graphical icons. The 
characters used were picked rather casually. If you know
of better characters to use for these purposes, please let
know.

Candidates will appear in the box to the right of the main area.

Click on the desired character to select it for pasting into
other programs. (Typically, pasting the selection is done in
X by clicking the center mouse button.)

To clear and start over, click on "消" (The lower button,
abbreviating "clear")

To quit select "Close" in the file menu.

"Save" in the character menu will write out the points in the character,
and the selected character to a file "samples.dat" in the 
current directory. This is intended for making a file of
characters for automated testing.

If "Annotate" in the character menu is selected, the strokes
will be annotated with their order.

## License

KanjiPad is Copyright 1997-1999, 2002 Owen Taylor, and licensed under
the terms of the GNU Public License. See the file COPYING for details.

See the file jstroke/readme.txt for information about the license
and copyright of JStroke.

Owen Taylor <otaylor@gtk.org></br>
October 27, 1997</br>
March   19, 1999 [ update for GTK+-1.2 ]</br>
August  25, 2002 [ update for GTK+-2.0 ]</br>
