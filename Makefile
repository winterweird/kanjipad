OPTIMIZE=-g -Wall
#OPTIMIZE=-O2 

GTKINC=$(shell pkg-config --cflags gtk+-2.0) -DG_DISABLE_DEPRECATED  -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED
GTKLIBS=$(shell pkg-config --libs gtk+-2.0)
GLIBLIBS=$(shell pkg-config --libs glib-2.0)

LIBS = -lm -lyajl_s

PREFIX=/usr/local
DATADIR=$(PREFIX)/share

# location in which binaries are installed
BINDIR=$(PREFIX)/bin
# location in which data files will be installed
LIBDIR=$(DATADIR)/kanjipad


# YAJL paths
# NOTE: Assuming the person building has git, cmake and make installed
YAJL_BASE = yajl/yajl-2.1.1
YAJL_LIB = $(YAJL_BASE)/lib
YAJL_INC = $(YAJL_BASE)/include

#
# On Win32, uncomment the following to avoid getting console windows
#
#LDFLAGS=-mwindows

INSTALL=install

####### No editing should be needed below here ##########

PACKAGE = kanjipad
VERSION = 2.0.0

OBJS = kpengine.o scoring.o util.o
CFLAGS = $(OPTIMIZE) -std=c99 -I$(YAJL_INC) $(GTKINC) -DFOR_PILOT_COMPAT -DKP_LIBDIR=\"$(LIBDIR)\" -DBINDIR=\"$(BINDIR)\"

all: kpengine kanjipad jdata.dat

$(YAJL_LIB): $(YAJL_BASE)
$(YAJL_INC): $(YAJL_BASE)

$(YAJL_BASE):
	-git clone git://github.com/lloyd/yajl
	cd yajl/ && cmake . &&  make

scoring.o: jstroke/scoring.c
	$(CC) -c -o scoring.o $(CFLAGS) $(LIBS) -Ijstroke jstroke/scoring.c 

util.o: jstroke/util.c
	$(CC) -c -o util.o $(CFLAGS) $(LIBS) -Ijstroke jstroke/util.c

kpengine: $(OBJS)
	$(CC) -o kpengine $(OBJS) $(GLIBLIBS) $(LDFLAGS) $(GTKLIBS)

kanjipad: kanjipad.o padarea.o karea.o global_vars.o callbacks.o engine.o sensitivity.o jisho_search.o json_to_gtk.o keyboard.o
	$(CC) -o kanjipad -I$(YAJL_INC) -L$(YAJL_LIB) $^ $(GTKLIBS) $(LDFLAGS) $(LIBS)
	
json_to_gtk.o: $(YAJL_LIB) $(YAJL_INC)

jdata.dat: jstroke/strokedata.h conv_jdata.pl
	perl conv_jdata.pl < jstroke/strokedata.h > jdata.dat

# using primitive commands because the install command apparently doesn't work
# on all systems
install: kanjipad kpengine jdata.dat
	mkdir -p $(DESTDIR)$(BINDIR)
	cp kanjipad $(DESTDIR)$(BINDIR)/kanjipad
	chmod 0755 $(DESTDIR)$(BINDIR)/kanjipad
	cp kpengine $(DESTDIR)$(BINDIR)/kpengine
	chmod 0755 $(DESTDIR)$(BINDIR)/kpengine
	
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp jdata.dat $(DESTDIR)$(LIBDIR)/jdata.dat
	chmod 0644 $(DESTDIR)$(LIBDIR)/jdata.dat
	
	mkdir -p ~/.kanjipad
	cp ui.xml ~/.kanjipad/ui.xml
	chmod 0644 ~/.kanjipad/ui.xml

clean:
	rm -rf *.o jdata.dat kpengine kanjipad yajl

$(PACKAGE).spec: $(PACKAGE).spec.in
	( sed s/@VERSION@/$(VERSION)/ < $< > $@.tmp && mv $@.tmp $@ ) || ( rm $@.tmp && false )

dist: $(PACKAGE).spec
	distdir=$(PACKAGE)-$(VERSION) ;					\
	tag=`echo $$distdir | tr a-z.- A-Z__` ;				\
	cvs tag -F $$tag &&						\
	cvs export -r $$tag -d $$distdir $(PACKAGE) &&			\
	cp $(PACKAGE).spec $$distdir &&					\
	tar cvf - $$distdir | gzip -c --best > $$distdir.tar.gz &&	\
	rm -rf $$distdir

distcheck: dist
	distdir=$(PACKAGE)-$(VERSION) ;		\
	tar xvfz $$distdir.tar.gz &&		\
	cd $$distdir &&				\
	make &&					\
	cd .. &&				\
	rm -rf $$distdir

.PHONY: dist distcheck
