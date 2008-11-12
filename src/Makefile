# J-Ben Makefile for Linux

# BUILD: either release, debug, or profile
BUILD = release
# JBEN_VERSION: Used for labeling release packages
JBEN_VERSION = $(shell cat version.h | grep VERSION_STR | sed -e 's/^[^"]*"\([^"]*\)"/\1/')
# PLATFORM: Used as a subdirectory name.
PLATFORM = linux

##########################################
# DO NOT EDIT BEYOND THIS POINT!         #
# (Unless you know what you're doing ;)) #
##########################################

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
build_depdir = ../dep/$(PLATFORM)/$(BUILD)
build_objdir = ../obj/$(PLATFORM)/$(BUILD)
build_bindir = ../bin/$(PLATFORM)/$(BUILD)

# compiler options
CXX = g++
CC = gcc
SharedCXXFLAGS = `pkg-config --cflags gtkmm-2.4 libxml-2.0`
SharedCPPFLAGS = -DJB_DATADIR=\"$(datadir)/jben\" \
	-DDOCDIR=\"$(docdir)/doc\" -DLICENSEDIR=\"$(docdir)/license\"
libs = `pkg-config --libs gtkmm-2.4 libxml-2.0` -lboost_regex

# C++ options
ifeq ($(BUILD),release)
	CXXFLAGS = -Wall -s -O2
	CPPFLAGS = -DNDEBUG #Added, used by assert
endif
ifeq ($(BUILD),debug)
	CXXFLAGS = -Wall -g
	CPPFLAGS = -DDEBUG
endif
#Profiling mode: DEBUG-style CXXFLAGS plus -pg, with RELEASE-style CPPFLAGS
ifeq ($(BUILD),profile)
	CXXFLAGS = -Wall -g -pg
	CPPFLAGS = -DNDEBUG #Added, used by assert
endif

sources = $(shell ls -t *.cpp) # Compile most recently edited files first.
objects = $(sources:%.cpp=$(build_objdir)/%.o)
target = jben

### Targets ###

all: $(build_bindir)/$(target) kpengine
	@echo
	@echo "J-Ben was built successfully."
	@echo "To install, use \"make install\" with the same options used to compile the"
	@echo "program."

.PHONY : kpengine clean cleandep cleanall dictdownload install uninstall doc

$(build_bindir)/$(target) : $(objects)
	mkdir -p $(build_bindir)
	$(CXX) $(CXXFLAGS) $(SharedCXXFLAGS) -o $@ $(objects) $(libs)

kpengine:
	cd kpengine && $(MAKE) BUILD=$(BUILD) && cd ..

clean:
	cd kpengine && $(MAKE) clean && cd ..
	rm -rfv ../bin ../obj ../jben.res

cleandep:
	rm -rfv ../dep

cleanall : clean cleandep

$(build_objdir)/%.o : %.cpp
	mkdir -p $(build_objdir)
	$(CXX) -c $(CPPFLAGS) $(SharedCPPFLAGS) $(CXXFLAGS) $(SharedCXXFLAGS) -o \
		$@ $(@F:%.o=%.cpp)

# Object dependency tracking
include $(sources:%.cpp=$(build_depdir)/%.d)
$(build_depdir)/%.d : %.cpp
	@echo Recreating $@...
	@mkdir -p $(build_depdir)
	@$(CXX) -MM $(CPPFLAGS) $< > $@.mktmp
	@sed 's,\($*\)\.o[ :]*,$(build_objdir)/\1.o $@ : ,g' < $@.mktmp > $@
	@rm $@.mktmp

download:
	./download_script

install:
	$(INSTALL) -d $(DESTDIR)$(bindir) $(DESTDIR)$(datadir)/jben/dicts \
		$(DESTDIR)$(datadir)/jben/sods $(DESTDIR)$(docdir)/license
	$(INSTALL_PROGRAM) $(build_bindir)/$(target) $(DESTDIR)$(bindir)
	$(INSTALL_DATA) ../dicts/* $(DESTDIR)$(datadir)/jben/dicts
	$(INSTALL_DATA) ../sods/README $(DESTDIR)$(datadir)/jben/sods
	$(if $(wildcard ../sods/sod-utf8-hex.tar.gz), \
		cd ../sods && tar -xzf sod-utf8-hex.tar.gz; \
		$(INSTALL) -d $(DESTDIR)$(datadir)/jben/sods/sod-utf8-hex; \
		$(INSTALL_DATA) ../sods/sod-utf8-hex/* \
			$(DESTDIR)$(datadir)/jben/sods/sod-utf8-hex; \
		cd ../sods && rm -rf sod-utf8-hex)
	$(if $(wildcard ../sods/soda-utf8-hex.tar.gz), \
		cd ../sods && tar -xzf soda-utf8-hex.tar.gz; \
		$(INSTALL) -d $(DESTDIR)$(datadir)/jben/sods/soda-utf8-hex; \
		$(INSTALL_DATA) ../sods/soda-utf8-hex/* \
			$(DESTDIR)$(datadir)/jben/sods/soda-utf8-hex; \
		cd ../sods && rm -rf soda-utf8-hex)
	$(INSTALL_DATA) ../license/* $(DESTDIR)$(docdir)/license
#	Install man page: $(INSTALL_DATA) ../<manpage> $(DESTDIR)$(man1dir)
	cd kpengine \
		&& $(MAKE) BUILD=$(BUILD) PLATFORM=$(PLATFORM) DESTDIR=$(DESTDIR) \
		install && cd ..
	@echo
	@echo "J-Ben was installed successfully."

uninstall:
	cd kpengine \
		&& $(MAKE) BUILD=$(BUILD) PLATFORM=$(PLATFORM) DESTDIR=$(DESTDIR) \
		uninstall && cd ..
	rm -f $(DESTDIR)$(bindir)/$(target)
	rm -f $(DESTDIR)$(datadir)/jben/dicts/*
	rm -f $(DESTDIR)$(datadir)/jben/sods/sod-utf8-hex/*
	rm -f $(DESTDIR)$(datadir)/jben/sods/soda-utf8-hex/*
	rm -f $(DESTDIR)$(datadir)/jben/sods/README
	rm -f $(DESTDIR)$(docdir)/license/*
#	rm -f $(DESTDIR)$(man1dir)/<manpage>
	rmdir -p $(DESTDIR)$(datadir)/jben/dicts 2> /dev/null || true
	rmdir -p $(DESTDIR)$(datadir)/jben/sods/sod-utf8-hex 2> /dev/null || true
	rmdir -p $(DESTDIR)$(datadir)/jben/sods/soda-utf8-hex 2> /dev/null || true
	rmdir -p $(DESTDIR)$(datadir)/jben/sods 2> /dev/null || true
	rmdir -p $(DESTDIR)$(docdir)/license 2> /dev/null || true
	@echo
	@echo "J-Ben was uninstalled successfully."

doc:
#	docbook2html errors since it expects SGML and I'm passing it XML.  Ignore the error.
	echo Compiling J-Ben $(JBEN_VERSION) documentation...
	rm -rf ../doc/html || true
	cd ../doc && (docbook2html -o html manual.xml || true) && cd ../src
	@echo '(Errors on docbook2html/openjade are expected; do not panic!)'
	cd ../doc && tar -cjf jben-$(JBEN_VERSION)-manual.tar.bz2 html && cd ../src
	cd ../doc && (rm -f jben-$(JBEN_VERSION)-manual.zip || true) && \
		zip -rq jben-$(JBEN_VERSION)-manual.zip html && cd ../src
