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
endif

# DO NOT EDIT BEYOND THIS POINT!

CXX = g++
target = jben

ifeq ($(PLATFORM),gtk20)
	SharedCXXFLAGS = `wx-config --cxxflags`
	SharedCPPFLAGS = `wx-config --cppflags`
	libs = `wx-config --libs`
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

#sources = $(wildcard *.cpp) # OLD, SIMPLE LINE
sources = $(shell ls -t *.cpp) # NEW LINE, using ls sorted by file modification time.  Makes most recently compiled files compile first.
objects = $(sources:.cpp=.o)

$(target) : $(objects)
	$(CXX) $(CXXFLAGS) -o $(target) $(objects) $(libs)

include $(sources:.cpp=.d)

clean:
	rm $(target) *.o

%.d : %.cpp
	@echo Recreating $@...
	@set -e; \
	rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
