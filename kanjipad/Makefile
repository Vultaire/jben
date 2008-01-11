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

KPADOBJDIR = obj/$(PLATFORM)/$(BUILD)
KPADBINDIR = bin/$(PLATFORM)/$(BUILD)

# Build object configuration
ifeq ($(PLATFORM),windows)
	kpengine = $(KPADBINDIR)/kpengine.exe
	mkdircmd = @mkdir
	rmdircmd = @rmdir /s
else
	kpengine = $(KPADBINDIR)/kpengine
	mkdircmd = @mkdir -p
	rmdircmd = @rm -rfv
endif

kpadsources = $(shell ls -t *.c) # Compile most recently edited files first.
kpadobjects = $(kpadsources:%.c=$(KPADOBJDIR)/%.o)

$(kpengine) : $(kpadobjects)
	echo Sources: $(kpadobjects)
	echo Objects: $(kpadobjects)
	$(mkdircmd) $(KPADBINDIR)
	$(CC) $(CFLAGS) -o $(kpengine) $(KPADOBJDIR)/kpengine.o $(KPADOBJDIR)/scoring.o $(KPADOBJDIR)/util.o

$(KPADOBJDIR)/%.o: %.c
	$(mkdircmd) $(KPADOBJDIR)
	$(CC) $(CFLAGS) -c -o $@ -DFOR_PILOT_COMPAT $<

clean:
	$(rmdircmd) bin
	$(rmdircmd) obj
