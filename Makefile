# RELEASE flags
CXXFLAGS = -Wall -s -O2 `wx-config --cxxflags`
CPPFLAGS = `wx-config --cppflags`

# DEBUG flags
##CXXFLAGS = -Wall -Werror -g `wx-config --cxxflags`
#CXXFLAGS = -Wall -g `wx-config --cxxflags`
#CPPFLAGS = -DDEBUG `wx-config --cppflags`

# Profiling mode: DEBUG flags, plus -pg, but without -DDEBUG
##CXXFLAGS = -Wall -Werror -g `wx-config --cxxflags` -pg
#CXXFLAGS = -Wall -g `wx-config --cxxflags` -pg
#CPPFLAGS = `wx-config --cppflags`

libs = `wx-config --libs`
target = jben

#sources = $(wildcard *.cpp) # OLD, SIMPLE LINE
sources = $(shell ls -t *.cpp) # NEW LINE, using ls sorted by file modification time.  Makes most recently compiled files compile first.
objects = $(sources:.cpp=.o)

$(target) : $(objects)
	g++ $(CXXFLAGS) -o $(target) $(objects) $(libs)

include $(sources:.cpp=.d)

clean:
	rm $(target) *.o

realclean:
	rm *~ $(target) *.o

%.d : %.cpp
	@echo Recreating $@...
	@set -e; \
	rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
