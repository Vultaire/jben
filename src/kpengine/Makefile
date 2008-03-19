# J-Ben Makefile for Linux and Windows
# Makefile for kpengine executable

### User-editable options ###
# PLATFORM: either windows or linux
PLATFORM = linux
# BUILD: either release, debug, or profile
BUILD = release

### C options ###
# These flags will be used for compiling the kpengine program, upon which our
# character handwriting recognition is dependent.
CC = gcc
CFLAGS = -Wall -s -O2

###########################
# Don't edit beyond here! #
###########################

# make options
SHELL = /bin/sh
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA    = ${INSTALL} -m 644
# From GNU Make manual, section 14.4: Variables for Installation Directories
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
datarootdir = $(prefix)/share
datadir = $(datarootdir)

# build dirs
build_objdir = ../../obj/$(PLATFORM)/$(BUILD)/kpengine
build_bindir = ../../bin/$(PLATFORM)/$(BUILD)/kpengine

# Build object configuration
ifeq ($(PLATFORM),windows)
	kpengine = jben_kpengine.exe
	# We don't want a popup console window on every call...
	kpengine_winflags = -mwindows
else
	kpengine = jben_kpengine
endif

sources = $(shell ls -t *.c) # Compile most recently edited files first.
objects = $(sources:%.c=$(build_objdir)/%.o)

.PHONY : clean install uninstall

$(build_bindir)/$(kpengine) : $(objects)
	echo Sources: $(objects)
	echo Objects: $(objects)
	mkdir -p $(build_bindir)
	$(CC) -o $(build_bindir)/$(kpengine) $(build_objdir)/kpengine.o \
		$(build_objdir)/scoring.o $(build_objdir)/util.o $(CFLAGS) $(kpengine_winflags)

$(build_objdir)/%.o: %.c
	mkdir -p $(build_objdir)
	$(CC) -c -o $@ -DFOR_PILOT_COMPAT $< $(CFLAGS)

clean:
	rm -rfv $(build_bindir) $(build_objdir)

install:
	$(INSTALL) -d $(DESTDIR)$(bindir) $(DESTDIR)$(datadir)/jben/kpengine_data
	$(INSTALL_PROGRAM) $(build_bindir)/$(kpengine) $(DESTDIR)$(bindir)
	$(INSTALL_DATA) *.unistrok $(DESTDIR)$(datadir)/jben/kpengine_data

uninstall:
	rm -fv $(DESTDIR)$(bindir)/$(kpengine)
	rm -rfv $(DESTDIR)$(datadir)/jben/kpengine_data
