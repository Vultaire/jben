# J-Ben Makefile for Linux and Windows

# Linux users: Ensure you have the development packages for libboost and
# wxWidgets installed.  Then, just "make".  There is no "make install" at this
# time, sorry.  It will be added later.

# Windows users:
# While Linux building should be mostly non-problematic, the Windows
# build support is NOT robust.  It's designed specifically for use with MinGW.
# If you want to give it a try, do the following:
# * Ensure mingw's bin folder is in your path
# * Install the coreutils and sed packages from the gnuwin32 project.
#   This means the binaries and the dependencies.  You'll probably come across
#   duplicate DLLs while unzipping these packages.  This should be okay, just
#   keep the newer copy of any duplicate DLLs.
# * Make sure the coreutils/sed binaries are in your path.
# * Change the 4 variables in the "WINDOWS-SPECIFIC" below as needed
# * build using the following command:
#   mingw32-make PLATFORM=windows BUILD=<release|debug|profile>

# PLATFORM: either windows or gtk20
PLATFORM = gtk20
# BUILD: either release, debug, or profile
BUILD = release

# WINDOWS-SPECIFIC
ifeq ($(PLATFORM),windows)
# CHANGE THESE 4 VARIABLES
	mingwbase = D:\MinGW
	boostbase = D:\boost_1_34_1
	wxbase = D:\wxMSW-2.8.4
	wxbuildflags = -DWXUSINGDLL -DwxUSE_UNICODE
# NOTE: wxWidgets is assumed to be built with wxUSINGDLL and wxUSE_UNICODE.
# Also, I'm currently using a release build of wxWidgets regardless of
# debug/release build flags - I may change this later.

# Below should all be okay, I think.
	wxplatformflags = -D__GNUWIN32__ -D__WXMSW__
	wincxxflags = -pipe -mthreads
	winlinkflags = -mwindows

	wxinclude = $(wxbase)\include
	wxcontribinc = $(wxbase)\contrib\include
	wxlibinc = $(wxbase)\lib\gcc_dll\mswu

	wxliblink = $(wxbase)\lib\gcc_dll
	SharedCXXFLAGS = $(wincxxflags)
	SharedCPPFLAGS = $(wxbuildflags) $(wxplatformflags) \
		-I$(wxinclude) -I$(wxcontribinc) -I$(wxlibinc) -I$(boostbase) -I$(mingwbase)\include
	libs = -L$(wxliblink) -L$(mingwbase)\lib -lwxmsw28u_html -lwxmsw28u_core \
		-lwxbase28u $(winlinkflags)
endif

# DO NOT EDIT BEYOND THIS POINT!
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

# These flags will be used for compiling the kpengine program, upon which our
# character handwriting recognition is dependent.
CC = gcc
CFLAGS = -Wall -s -O2

#sources = $(wildcard *.cpp) # OLD, SIMPLE LINE
sources = $(shell ls -t *.cpp) # NEW LINE, using ls sorted by file modification time.  Makes most recently compiled files compile first.

ifeq ($(PLATFORM),windows)
	objects = $(sources:.cpp=.o) jben.res
	target = jben.exe
	kpengine = kpengine.exe
else
	objects = $(sources:.cpp=.o)
	target = jben
	kpengine = kpengine
endif

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
	windres.exe -i jben.rc -J rc -o jben.res -O coff -I$(wxinclude) -I$(wxlibinc) -I$(mingwbase)\include

clean:
	rm $(target) $(kpengine) *.o jben.res

include $(sources:.cpp=.d)
%.d : %.cpp
	@echo Recreating $@...
ifeq ($(PLATFORM),windows)
	@$(CXX) -MM $(CPPFLAGS) $< > $@.mktmp
	@sed "s,\($*\)\.o[ :]*,\1.o $@ : ,g" < $@.mktmp > $@
	@rm -f $@.mktmp
else
	@set -e; \
	rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
endif
