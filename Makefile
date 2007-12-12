# PLATFORM: either windows or gtk20
PLATFORM = gtk20
# BUILD: either release, debug, or profile
BUILD = release

# WINDOWS-SPECIFIC
# If building for Windows, the following vars must be explicitly set.
# We need appropriate cxxflags/cppflags/libs for wxWidgets and any other
# libraries/toolkits being used.  Define them here; they're used for all
# builds.
ifeq ($(PLATFORM),windows)
	SharedCXXFLAGS = 
	SharedCPPFLAGS = 
	libs = 
	kpengine = kpengine.exe
endif

# DO NOT EDIT BEYOND THIS POINT!

CXX = g++
target = jben

ifeq ($(PLATFORM),gtk20)
	SharedCXXFLAGS = `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0`
	SharedCPPFLAGS = `wx-config --cppflags`
	libs = `wx-config --libs` `pkg-config --libs gtk+-2.0`
	kpengine = kpengine
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
kpengine = kpengine
CC = gcc
CFLAGS = -Wall -s -O2

#sources = $(wildcard *.cpp) # OLD, SIMPLE LINE
sources = $(shell ls -t *.cpp) # NEW LINE, using ls sorted by file modification time.  Makes most recently compiled files compile first.
objects = $(sources:.cpp=.o)

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


clean:
	rm $(target) *.o

include $(sources:.cpp=.d)
%.d : %.cpp
	@echo Recreating $@...
	@set -e; \
	rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
