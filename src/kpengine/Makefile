# J-Ben Makefile for Linux and Windows
# Makefile for kpengine executable

### User-editable options ###
# PLATFORM: either windows or gtk20
PLATFORM = gtk20
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
	mkdircmd  = C:/progra~1/gnuwin32/bin/mkdir -p
	rmdircmd = rmdir /s
else
	kpengine = jben_kpengine
	mkdircmd = mkdir -p
	rmdircmd = rm -rfv
endif

sources = $(shell ls -t *.c) # Compile most recently edited files first.
objects = $(sources:%.c=$(build_objdir)/%.o)

.PHONY : clean install uninstall

$(build_bindir)/$(kpengine) : $(objects)
	echo Sources: $(objects)
	echo Objects: $(objects)
	$(mkdircmd) $(build_bindir)
	$(CC) -o $(build_bindir)/$(kpengine) $(build_objdir)/kpengine.o \
		$(build_objdir)/scoring.o $(build_objdir)/util.o $(CFLAGS)

$(build_objdir)/%.o: %.c
	$(mkdircmd) $(build_objdir)
	$(CC) -c -o $@ -DFOR_PILOT_COMPAT $< $(CFLAGS)

clean:
	$(rmdircmd) bin obj

install:
	$(INSTALL) -d $(DESTDIR)$(bindir) $(DESTDIR)$(datadir)/jben/kpengine_data
	$(INSTALL_PROGRAM) $(build_bindir)/$(kpengine) $(DESTDIR)$(bindir)
	$(INSTALL_DATA) *.unistrok $(DESTDIR)$(datadir)/jben/kpengine_data

uninstall:
	rm -fv $(DESTDIR)$(bindir)/$(kpengine)
	rm -rfv $(DESTDIR)$(datadir)/jben/kpengine_data
