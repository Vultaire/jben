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
# whether a debug or release build is chosen.  I may change this later.

# The following variables probably should NOT be changed.
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

# make options
SHELL = /bin/sh
OBJDIR = obj/$(PLATFORM)/$(BUILD)
BINDIR = bin/$(PLATFORM)/$(BUILD)
DEPDIR = dep/$(PLATFORM)/$(BUILD)

# Get flags/libs for GTK builds
ifeq ($(PLATFORM),gtk20)
	SharedCXXFLAGS = `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0`
	SharedCPPFLAGS = `wx-config --cppflags`
	libs = `wx-config --libs` `pkg-config --libs gtk+-2.0`
endif

# C++ options
CXX = g++
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

sources = $(shell ls -t *.cpp) # Compile most recently edited files first.

# Build object configuration
ifeq ($(PLATFORM),windows)
	objects = $(sources:%.cpp=$(OBJDIR)/%.o) $(OBJDIR)/jben.res
	target = $(BINDIR)/jben.exe
else
	objects = $(sources:%.cpp=$(OBJDIR)/%.o)
	target = $(BINDIR)/jben
endif

# Select build environment type
ifeq ($(CANUCK),1)
	buildenv = posix
else

ifeq ($(PLATFORM),windows)
	buildenv = windows
else
	buildenv = posix
endif

endif

# Define commands based on build environment
ifeq ($(buildenv),windows)
	mkdircmd = mkdir
else
	mkdircmd = mkdir -p
endif

### Targets ###

all: $(target) kpengine

.PHONY : kpengine clean cleandep cleanall

$(target) : $(objects)
	$(mkdircmd) $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $(target) $(objects) $(libs)


$(OBJDIR)/jben.res:
	$(mkdircmd) $(OBJDIR)
	windres.exe -i jben.rc -J rc -o $(OBJDIR)/jben.res -O coff -I$(wxinclude) -I$(wxlibinc) -I$(mingwbase)/include

kpengine:
	cd kanjipad && make && cd ..

clean:
	cd kanjipad && make clean && cd ..
	rm -rfv bin obj jben.res

cleandep:
	rm -rfv dep

cleanall : clean cleandep

$(OBJDIR)/%.o : %.cpp
	$(mkdircmd) $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $(@F:%.o=%.cpp)

# Object dependency tracking
include $(sources:%.cpp=$(DEPDIR)/%.d)
$(DEPDIR)/%.d : %.cpp
	@echo Recreating $@...
	@$(mkdircmd) $(DEPDIR)
	@$(CXX) -MM $(CPPFLAGS) $< > $@.mktmp
ifeq ($(buildenv),windows)
	@sed "s,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g" < $@.mktmp > $@
else
#	@sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : $(OBJDIR) ,g' < $@.mktmp > $@
	@sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' < $@.mktmp > $@
endif
	@rm $@.mktmp
