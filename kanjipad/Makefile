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

OBJDIR = obj/$(PLATFORM)/$(BUILD)
BINDIR = bin/$(PLATFORM)/$(BUILD)

# Build object configuration
ifeq ($(PLATFORM),windows)
	kpengine = $(BINDIR)/kpengine.exe
	mkdircmd  = C:/progra~1/gnuwin32/bin/mkdir -p
	rmdircmd = rmdir /s
else
	kpengine = $(BINDIR)/kpengine
	mkdircmd = mkdir -p
	rmdircmd = rm -rfv
endif

sources = $(shell ls -t *.c) # Compile most recently edited files first.
objects = $(sources:%.c=$(OBJDIR)/%.o)

.PHONY : clean

$(kpengine) : $(objects)
	echo Sources: $(objects)
	echo Objects: $(objects)
	$(mkdircmd) $(BINDIR)
	$(CC) $(CFLAGS) -o $(kpengine) $(OBJDIR)/kpengine.o $(OBJDIR)/scoring.o $(OBJDIR)/util.o

$(OBJDIR)/%.o: %.c
	$(mkdircmd) $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ -DFOR_PILOT_COMPAT $<

clean:
	$(rmdircmd) bin
	$(rmdircmd) obj
