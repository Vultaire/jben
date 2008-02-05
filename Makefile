# J-Ben Makefile for Linux and Windows

### User-editable options ###
# PLATFORM: either windows or gtk20
PLATFORM = gtk20
# BUILD: either release, debug, or profile
BUILD = release
# MAKE: change the make command, if needed

# WINDOWS-SPECIFIC
ifeq ($(PLATFORM),windows)
# CHANGE THESE 4 VARIABLES
	MAKE = mingw32-make
	mingwbase = C:/dev/MinGW
	boostbase = C:/dev/boost_1_34_1
	wxbase = C:/dev/wxMSW-2.8.7
	wxbuildflags = -DWXUSINGDLL -DwxUSE_UNICODE
	stlportbase = C:/dev/STLport-5.1.5
	iconvbase = C:/dev/libiconv
	mkdircmd  = C:/progra~1/gnuwin32/bin/mkdir -p
# NOTE: wxWidgets is assumed to be built with wxUSINGDLL and wxUSE_UNICODE.
# Also, I'm currently using a release build of wxWidgets regardless of
# whether a debug or release build is chosen.  I may change this later.

# The following variables probably should NOT be changed.
	wxplatformflags = -D__GNUWIN32__ -D__WXMSW__
	# STLPort specifies -pthreads - does -mthreads work fine for this???
	wincxxflags = -pipe -mthreads
	winlinkflags = -mwindows

	wxinclude = $(wxbase)/include
	wxcontribinc = $(wxbase)/contrib/include
	wxlibinc = $(wxbase)/lib/gcc_dll/mswu

	wxliblink = $(wxbase)/lib/gcc_dll
	SharedCXXFLAGS = $(wincxxflags)
	SharedCPPFLAGS = -I$(stlportbase)/stlport $(wxbuildflags) \
		$(wxplatformflags) -I$(wxinclude) -I$(wxcontribinc) -I$(wxlibinc) \
		-I$(boostbase) -I$(mingwbase)/include -I$(iconvbase)/include
	libs = -L$(stlportbase)/lib -L$(wxliblink) -L$(mingwbase)/lib -L$(iconvbase)/lib \
		-lwxmsw28u_html -lwxmsw28u_core -lwxbase28u $(winlinkflags) -liconv -lstlport.5.1.dll
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
	MAKE = make
	SharedCXXFLAGS = `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0`
	SharedCPPFLAGS = `wx-config --cppflags`
	libs = `wx-config --libs` `pkg-config --libs gtk+-2.0`
	mkdircmd = mkdir -p
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
	cd kanjipad && $(MAKE) BUILD=$(BUILD) PLATFORM=$(PLATFORM) && cd ..

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
