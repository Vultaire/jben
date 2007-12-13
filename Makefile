# J-Ben Makefile for Linux and Windows

### User-editable options ###
# PLATFORM: either windows or gtk20
PLATFORM = gtk20
# BUILD: either release, debug, or profile
BUILD = release

# WINDOWS-SPECIFIC
ifeq ($(PLATFORM),windows)
# CHANGE THESE 4 VARIABLES
	mingwbase = D:/MinGW
	boostbase = D:/boost_1_34_1
	wxbase = D:/wxMSW-2.8.7
	wxbuildflags = -DWXUSINGDLL -DwxUSE_UNICODE
# NOTE: wxWidgets is assumed to be built with wxUSINGDLL and wxUSE_UNICODE.
# Also, I'm currently using a release build of wxWidgets regardless of
# debug/release build flags - I may change this later.

	wxplatformflags = -D__GNUWIN32__ -D__WXMSW__
	wincxxflags = -pipe -mthreads
	winlinkflags = -mwindows

	wxinclude = $(wxbase)/include
	wxcontribinc = $(wxbase)/contrib/include
	wxlibinc = $(wxbase)/lib/gcc_dll/mswu

	wxliblink = $(wxbase)/lib/gcc_dll
	SharedCXXFLAGS = $(wincxxflags)
	SharedCPPFLAGS = $(wxbuildflags) $(wxplatformflags) \
		-I$(wxinclude) -I$(wxcontribinc) -I$(wxlibinc) -I$(boostbase) -I$(mingwbase)/include
	libs = -L$(wxliblink) -L$(mingwbase)/lib -lwxmsw28u_html -lwxmsw28u_core \
		-lwxbase28u $(winlinkflags)
endif

##################################
# DO NOT EDIT BEYOND THIS POINT! #
##################################

### make options ###
SHELL = /bin/sh

### C++ options ###
CXX = g++

ifeq ($(PLATFORM),gtk20)
	SharedCXXFLAGS = `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0`
	SharedCPPFLAGS = `wx-config --cppflags`
	libs = `wx-config --libs` `pkg-config --libs gtk+-2.0`
endif

ifeq ($(BUILD),release)
	CXXFLAGS = -Wall -s -O2 $(SharedCXXFLAGS)
	CPPFLAGS = $(SharedCPPFLAGS)
endif
ifeq ($(BUILD),debug)
	CXXFLAGS = -Wall -g $(SharedCXXFLAGS)
	CPPFLAGS = -DDEBUG $(SharedCPPFLAGS)
endif
ifeq ($(BUILD),profile)
# Profiling mode: DEBUG flags, plus -pg, but without -DDEBUG
	CXXFLAGS = -Wall -g $(SharedCXXFLAGS) -pg
	CPPFLAGS = $(SharedCPPFLAGS)
endif

### C options ###
# These flags will be used for compiling the kpengine program, upon which our
# character handwriting recognition is dependent.
CC = gcc
CFLAGS = -Wall -s -O2

### Build object configuration ###
sources = $(shell ls -t *.cpp) # Compile most recently edicted files first.

ifeq ($(PLATFORM),windows)
	objects = $(sources:.cpp=.o) jben.res
	target = jben.exe
	kpengine = kpengine.exe
else
	objects = $(sources:.cpp=.o)
	target = jben
	kpengine = kpengine
endif

### Targets ###
all: $(target) $(kpengine)

$(target) : $(objects)
	$(CXX) $(CXXFLAGS) -o $(target) $(objects) $(libs)

$(kpengine) : kpengine.o scoring.o util.o
	$(CC) $(CFLAGS) -o $(kpengine) kpengine.o scoring.o util.o
kpengine.o: kanjipad/kpengine.c
	$(CC) $(CFLAGS) -c -o kpengine.o -DFOR_PILOT_COMPAT -Ikanjipad kanjipad/kpengine.c
scoring.o: kanjipad/scoring.c
	$(CC) $(CFLAGS) -c -o scoring.o -DFOR_PILOT_COMPAT -Ikanjipad/jstroke kanjipad/scoring.c
util.o: kanjipad/util.c
	$(CC) $(CFLAGS) -c -o util.o -DFOR_PILOT_COMPAT -Ikanjipad/jstroke kanjipad/util.c

jben.res:
	windres.exe -i jben.rc -J rc -o jben.res -O coff -I$(wxinclude) -I$(wxlibinc) -I$(mingwbase)/include

clean:
	rm $(target) $(kpengine) *.o jben.res

### Object dependency tracking ###
include $(sources:.cpp=.d)
%.d : %.cpp
	@echo Recreating $@...
ifeq ($(PLATFORM),windows)
ifeq ($(CANUCK)),1)	# Canadian cross: Linux->MinGW compilation
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
else # Windows build using native MinGW and gnuwin32 coreutils/sed
	@$(CXX) -MM $(CPPFLAGS) $< > $@.mktmp
	@sed "s,\($*\)\.o[ :]*,\1.o $@ : ,g" < $@.mktmp > $@
	@rm -f $@.mktmp
endif
else # Standard build on Linux
	@$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
endif
