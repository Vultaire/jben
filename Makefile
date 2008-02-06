# J-Ben Makefile for Linux and Windows

### User-editable options ###
# PLATFORM: either windows or gtk20
PLATFORM = gtk20
# BUILD: either release, debug, or profile
BUILD = release
# MAKE: change the make command, if needed

# WINDOWS-SPECIFIC
ifeq ($(PLATFORM),windows)
#	CHANGE THESE 4 VARIABLES
	MAKE = mingw32-make
	mingwbase = C:/dev/MinGW
	boostbase = C:/dev/boost_1_34_1
	wxbase = C:/dev/wxMSW-2.8.7
	wxbuildflags = -DWXUSINGDLL -DwxUSE_UNICODE
	stlportbase = C:/dev/STLport-5.1.5
	iconvbase = C:/dev/libiconv
	mkdircmd  = C:/progra~1/gnuwin32/bin/mkdir -p
#	NOTE: wxWidgets is assumed to be built with wxUSINGDLL and wxUSE_UNICODE.
#	Also, I'm currently using a release build of wxWidgets regardless of
#	whether a debug or release build is chosen.  I may change this later.

#	The following variables probably should NOT be changed.
	wxplatformflags = -D__GNUWIN32__ -D__WXMSW__
#	STLPort specifies -pthreads - does -mthreads work fine for this???
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
	libs = -L$(stlportbase)/lib -L$(wxliblink) -L$(mingwbase)/lib \
		-L$(iconvbase)/lib -lwxmsw28u_html -lwxmsw28u_core -lwxbase28u \
		$(winlinkflags) -liconv -lstlport.5.1.dll
endif

##################################
# DO NOT EDIT BEYOND THIS POINT! #
##################################

# make options
SHELL = /bin/sh
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA    = $(INSTALL) -m 644
# From GNU Make manual, section 14.4: Variables for Installation Directories
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
datarootdir = $(prefix)/share
datadir = $(datarootdir)
docdir = $(datarootdir)/doc/jben
# I don't have a man page written yet, but I'll define the vars anyway.
mandir = $(datarootdir)/man
man1dir = $(mandir)/man1

# build dirs
build_depdir = dep/$(PLATFORM)/$(BUILD)
build_objdir = obj/$(PLATFORM)/$(BUILD)
build_bindir = bin/$(PLATFORM)/$(BUILD)


# Get flags/libs for GTK builds
ifeq ($(PLATFORM),gtk20)
	MAKE = make
	SharedCXXFLAGS = `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0`
	SharedCPPFLAGS = `wx-config --cppflags` -DDATADIR=\"$(datadir)/jben\" \
		-DDOCDIR=\"$(docdir)/doc\" -DLICENSEDIR=\"$(docdir)/license\"
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
#	Profiling mode: DEBUG flags, plus -pg, but without -DDEBUG
	CXXFLAGS = -Wall -g $(SharedCXXFLAGS) -pg
	CPPFLAGS = $(SharedCPPFLAGS)
endif

sources = $(shell ls -t *.cpp) # Compile most recently edited files first.

# Build object configuration
ifeq ($(PLATFORM),windows)
	objects = $(sources:%.cpp=$(build_objdir)/%.o) $(build_objdir)/jben.res
	target = jben.exe
else
	objects = $(sources:%.cpp=$(build_objdir)/%.o)
	target = jben
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

all: $(build_bindir)/$(target) kpengine

.PHONY : kpengine clean cleandep cleanall install uninstall

$(build_bindir)/$(target) : $(objects)
	$(mkdircmd) $(build_bindir)
	$(CXX) -o $(build_bindir)/$(target) $(objects) $(libs) $(CXXFLAGS)


$(build_objdir)/jben.res:
	$(mkdircmd) $(build_objdir)
	windres.exe -i jben.rc -J rc -o $(build_objdir)/jben.res -O coff -I$(wxinclude) -I$(wxlibinc) -I$(mingwbase)/include

kpengine:
	cd kanjipad && $(MAKE) BUILD=$(BUILD) PLATFORM=$(PLATFORM) && cd ..

clean:
	cd kanjipad && make clean && cd ..
	rm -rfv bin obj jben.res

cleandep:
	rm -rfv dep

cleanall : clean cleandep

$(build_objdir)/%.o : %.cpp
	$(mkdircmd) $(build_objdir)
	$(CXX) $(CPPFLAGS) -c -o $@ $(@F:%.o=%.cpp) $(CXXFLAGS) 

# Object dependency tracking
include $(sources:%.cpp=$(build_depdir)/%.d)
$(build_depdir)/%.d : %.cpp
	@echo Recreating $@...
	@$(mkdircmd) $(build_depdir)
	@$(CXX) -MM $(CPPFLAGS) $< > $@.mktmp
ifeq ($(buildenv),windows)
	@sed "s,\($*\)\.o[ :]*,$(build_objdir)/\1.o $@ : ,g" < $@.mktmp > $@
else
	@sed 's,\($*\)\.o[ :]*,$(build_objdir)/\1.o $@ : ,g' < $@.mktmp > $@
endif
	@rm $@.mktmp

install:
	$(INSTALL) -d $(bindir) $(datadir)/jben/dicts $(datadir)/jben/sods \
		$(docdir)/license
	$(INSTALL_PROGRAM) $(build_bindir)/$(target) $(bindir)
	$(INSTALL_DATA) dicts/* $(datadir)/jben/dicts
	$(INSTALL_DATA) sods/README.txt $(datadir)/jben/sods
	$(INSTALL_DATA) license/* $(docdir)/license
#	Install man page: $(INSTALL_DATA) <manpage> $(man1dir)
	cd kanjipad \
		&& $(MAKE) BUILD=$(BUILD) PLATFORM=$(PLATFORM) install \
		&& cd ..
	@echo
	@echo "J-Ben was installed successfully."

uninstall:
	cd kanjipad \
		&& $(MAKE) BUILD=$(BUILD) PLATFORM=$(PLATFORM) uninstall \
		&& cd ..
	rm -fv $(bindir)/$(target)
	rm -fv $(datadir)/jben/dicts/*
	rm -fv $(datadir)/jben/sods/README.txt
	rm -fv $(docdir)/license/*
#	rm -fv $(man1dir)/<manpage>
	rmdir -p $(datadir)/jben/dicts || true
	rmdir -p $(datadir)/jben/sods || true
	rmdir -p $(docdir)/license || true
	@echo
	@echo "J-Ben was uninstalled successfully."
