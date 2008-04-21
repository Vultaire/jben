# J-Ben Makefile for Linux and Windows

# NOTES FOR WINDOWS:
# Windows builds expect MinGW WITH MSys,
# one of the tech. preview gcc 4 compilers (for wstrings),
# plus 7-Zip, NSIS and GTK binaries in your path for "make install".
# PKG_CONFIG_PATH must also be set appropriately.

### User-editable options ###
# PLATFORM: either windows or linux
PLATFORM = linux
# BUILD: either release, debug, or profile
BUILD = release
# JBEN_VERSION: Used for labeling release packages
JBEN_VERSION = 1.2.0

# WINDOWS-SPECIFIC
ifeq ($(PLATFORM),windows)
#	===CHANGE THESE VARIABLES===
	CXX = g++-dw2
	CC = gcc-dw2
	mingwbase = C:/MinGW
	gtkdevbase = C:/GTK
	boostbase = C:/devel/boost_1_35_0
#	Path to GNU iconv development files (works faster than win_iconv, from my tests)
	iconvbase = C:/devel/libiconv
#	The below 2 vars should be direct paths to the boost::regex library and the zlib library.
	libboost_regex = C:/devel/boost-build-gcc4dw2/boost/bin.v2/libs/regex/build/gcc-mingw-4.2.1/release/link-static/threading-multi/libboost_regex-mgw42-mt-1_35.lib
	libzlib = C:/GTK/lib/zdll.lib
#	The below variables point towards the path to a configured,
#	redistributable GTK/GTKmm environment.  It's used by the Windows
#	"make install" target.  If you need help setting this up, contact the
#	author directly.
	gtkredist = C:/gtk-redist

#   ===LEAVE THESE ALONE===
	wincxxflags = -I$(iconvbase)/include -I$(gtkdevbase)/include `pkg-config --cflags gtkmm-2.4 libxml-2.0`
	wincppflags = -D__WIN32__
	winlinkflags = -mwindows

	SharedCXXFLAGS = $(wincxxflags)
	SharedCPPFLAGS = $(wincppflags) -I$(boostbase)
	libs = $(winlinkflags) `pkg-config --libs gtkmm-2.4 libxml-2.0` -L$(iconvbase)/lib -liconv.dll $(libboost_regex) $(libzlib)
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
build_depdir = ../dep/$(PLATFORM)/$(BUILD)
build_objdir = ../obj/$(PLATFORM)/$(BUILD)
build_bindir = ../bin/$(PLATFORM)/$(BUILD)


# Get flags/libs for Linux builds
ifeq ($(PLATFORM),linux)
	CXX = g++
	CC = gcc
	SharedCXXFLAGS = `pkg-config --cflags gtkmm-2.4 libxml-2.0`
	SharedCPPFLAGS = -DJB_DATADIR=\"$(datadir)/jben\" \
		-DDOCDIR=\"$(docdir)/doc\" -DLICENSEDIR=\"$(docdir)/license\"
	libs = `pkg-config --libs gtkmm-2.4 libxml-2.0` -lboost_regex
endif

# C++ options
ifeq ($(BUILD),release)
	CXXFLAGS = -Wall -s -O2
	CPPFLAGS = -DNDEBUG #Added, used by assert
endif
ifeq ($(BUILD),debug)
	CXXFLAGS = -Wall -g
	CPPFLAGS = -DDEBUG
endif
ifeq ($(BUILD),profile)
#	Profiling mode: DEBUG flags, plus -pg, but without -DDEBUG
	CXXFLAGS = -Wall -g -pg
endif

sources = $(shell ls -t *.cpp) # Compile most recently edited files first.

# Build object configuration
ifeq ($(PLATFORM),windows)
	objects = $(sources:%.cpp=$(build_objdir)/%.o) $(build_objdir)/jben-rc.o
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
ifeq ($(PLATFORM),windows)
	@echo J-Ben was built successfully.
else
	@echo
	@echo "J-Ben was built successfully."
	@echo "To install, use \"make install\" with the same options used to compile the"
	@echo "program."
endif

.PHONY : kpengine clean cleandep cleanall installprep install uninstall doc

$(build_bindir)/$(target) : $(objects)
	mkdir -p $(build_bindir)
	$(CXX) $(CXXFLAGS) $(SharedCXXFLAGS) -o $@ $(objects) $(libs)

kpengine:
	cd kpengine && $(MAKE) BUILD=$(BUILD) PLATFORM=$(PLATFORM) && cd ..

clean:
	cd kpengine && $(MAKE) clean && cd ..
	rm -rfv ../bin ../obj ../jben.res

cleandep:
	rm -rfv ../dep

cleanall : clean cleandep

$(build_objdir)/jben-rc.o:
	mkdir -p $(build_objdir)
	windres.exe -o $@ jben.rc

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
ifeq ($(buildenv),windows)
	@sed "s,\($*\)\.o[ :]*,$(build_objdir)/\1.o $@ : ,g" < $@.mktmp > $@
else
	@sed 's,\($*\)\.o[ :]*,$(build_objdir)/\1.o $@ : ,g' < $@.mktmp > $@
endif
	@rm $@.mktmp

installprep:
ifeq ($(PLATFORM),windows)
#	Make a phony output folder with all redist files
	mkdir -p ../J-Ben
#	Copy GTK files
	cp -R $(gtkredist)/bin ../J-Ben
	cp -R $(gtkredist)/etc ../J-Ben
	cp -R $(gtkredist)/lib ../J-Ben
	cp -R $(gtkredist)/share ../J-Ben
#	Copy GNU iconv files
	cp -R $(iconvbase)/bin/libiconv2.dll ../J-Ben/bin
#	Copy J-Ben binaries and DLLs not in GTK redist folder
	cp "$(mingwbase)/bin/mingwm10.dll" ../J-Ben/bin
	cp "$(build_bindir)/$(target)" ../J-Ben/bin
	cp "$(build_bindir)/kpengine/jben_kpengine.exe" ../J-Ben/bin
#	Strip J-Ben executables
	strip ../J-Ben/bin/$(target)
	strip ../J-Ben/bin/jben_kpengine.exe
#	Copy remaining J-Ben files (data, readmes, licenses, docs, etc.)
	cp ../*.txt ../J-Ben
	cp -R ../license ../J-Ben
	mkdir -p ../J-Ben/kpengine_data ../J-Ben/dicts ../J-Ben/sods
	cp kpengine/*.unistrok ../J-Ben/kpengine_data
	rm -rf ../J-Ben/doc
	cp -R ../doc/html ../J-Ben/doc || true
	cp ../dicts/* ../J-Ben/dicts
	cp ../sods/README.txt ../J-Ben/sods
	@echo J-Ben distribution created in ..\\J-Ben.
endif

install : installprep
ifeq ($(PLATFORM),windows)
#	Zip the output folder into 7z and zip archives
	mkdir -p ../archives/$(JBEN_VERSION)
	rm ../archives/$(JBEN_VERSION)/jben-$(JBEN_VERSION)-win-binary.zip || true
	rm ../archives/$(JBEN_VERSION)/jben-$(JBEN_VERSION)-win-binary.7z || true
	7z a -tzip ../archives/$(JBEN_VERSION)/jben-$(JBEN_VERSION)-win-binary.zip \
		../J-Ben
	7z a -t7z ../archives/$(JBEN_VERSION)/jben-$(JBEN_VERSION)-win-binary.7z \
		../J-Ben
#####
# Msys has issues with programs requiring "/" switches.  So, makensis with a /D switch won't fly until I can find a workaround.
#####
	@echo To create an installer, open a normal command prompt, navigate to this
	@echo directory, and type the following:
	@echo makensis /Dversion=$(JBEN_VERSION) installer.nsi
	@echo \(Afterwards, delete ..\J-Ben.\)
##	Run NSIS script
#	makensis /Dversion=$(JBEN_VERSION) installer.nsi
##	Remove the output folder
#	rm -rf ../J-Ben
else
	$(INSTALL) -d $(DESTDIR)$(bindir) $(DESTDIR)$(datadir)/jben/dicts \
		$(DESTDIR)$(datadir)/jben/sods $(DESTDIR)$(docdir)/license
	$(INSTALL_PROGRAM) $(build_bindir)/$(target) $(DESTDIR)$(bindir)
	$(INSTALL_DATA) ../dicts/* $(DESTDIR)$(datadir)/jben/dicts
	$(INSTALL_DATA) ../sods/README.txt $(DESTDIR)$(datadir)/jben/sods
	$(INSTALL_DATA) ../license/* $(DESTDIR)$(docdir)/license
#	Install man page: $(INSTALL_DATA) ../<manpage> $(DESTDIR)$(man1dir)
	cd kpengine \
		&& $(MAKE) BUILD=$(BUILD) PLATFORM=$(PLATFORM) DESTDIR=$(DESTDIR) \
		install && cd ..
	@echo
	@echo "J-Ben was installed successfully."
endif

uninstall:
ifeq ($(PLATFORM),windows)
	@echo "make uninstall" is not supported on Windows.
else
	cd kpengine \
		&& $(MAKE) BUILD=$(BUILD) PLATFORM=$(PLATFORM) DESTDIR=$(DESTDIR) \
		uninstall && cd ..
	rm -fv $(DESTDIR)$(bindir)/$(target)
	rm -fv $(DESTDIR)$(datadir)/jben/dicts/*
	rm -fv $(DESTDIR)$(datadir)/jben/sods/README.txt
	rm -fv $(DESTDIR)$(docdir)/license/*
#	rm -fv $(DESTDIR)$(man1dir)/<manpage>
	rmdir -p $(DESTDIR)$(datadir)/jben/dicts || true
	rmdir -p $(DESTDIR)$(datadir)/jben/sods || true
	rmdir -p $(DESTDIR)$(docdir)/license || true
	@echo
	@echo "J-Ben was uninstalled successfully."
endif

doc:
#	docbook2html errors since it expects SGML and I'm passing it XML.  Ignore the error.
	rm -rfv ../doc/html || true
	cd ../doc && (docbook2html -o html manual.xml || true) && cd ../src
	@echo '(Errors on docbook2html/openjade are expected; do not panic!)'
	cd ../doc && tar -cjf jben-$(JBEN_VERSION)-manual.tar.bz2 html && cd ../src
	cd ../doc && (rm jben-$(JBEN_VERSION)-manual.zip || true) && \
		zip -rq jben-$(JBEN_VERSION)-manual.zip html && cd ../src
