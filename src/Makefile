#
#
#          Sean Johnson
#        December 23, 2019
#
#

# PERSONAL MAKEFILE TEMPLATE
# Only the variables should be changed, and some of those do not need to be

# This template was created with the following project structure in mind:
# - root
#   - Makefile
#   - bin/ (Output files, Generated)
#   - include/ (Libraries)
#   - src/ (Project sources)
#   - tests/ (Project tests, I hope you are using these)

#universal
MAINFILE = main.cpp #needed so it is not compiled with everything else during the tests
#(Note that the main function sohuld be separate from everything else in a large project)
PROJECT = calc
COMPILER = g++
INCLUDEDIR = ./include
SRCBIN = ./bin
OUTPUTFILE = $(SRCBIN)/$(PROJECT)

#sources
SRCDIR = ./src
SRCFILES = $(shell find $(SRCDIR) -name "*.cpp")
SRCLIBS = 
SRCOPTIONS = -std=c++11

#tests
TESTDIR = ./tests
TESTSRCFILES = $(shell find $(TESTDIR) $(SRCDIR) -name "*.cpp" -not -name $(MAINFILE))
TESTLIBS = -lgtest -lgtest_main
TESTOUTDIR = ./tests/out
TESTOUTFILE = $(TESTOUTDIR)/test
TESTOPTIONS = -std=c++11

#objects
OBJDIR = ./bin/obj
OBJECTS = $(subst ./src/%.cpp,./bin/obj/%.o,$(SRCFILES))

all: clean compile test

compile: clean $(OBJECTS)
	#Create needed directories
	mkdir $(SRCBIN)
	#Link them
	$(COMPILER) -c $(OBJECTS) -o $(OUTPUTFILE) $(SRCLIBS) $(SRCOPTIONS)
	#Done compiling

test: clean
	#Create needed directories
	mkdir $(TESTOUTDIR)
	#Compile (I need to compile the sources with the tests)
	$(COMPILER) $(TESTSRCFILES) -o $(TESTOUTFILE) $(TESTLIBS) $(TESTOPTIONS)
	#Run
	./$(TESTOUTFILE)

$(OBJECTS): %.cpp

depend: .depend

.depend: $(SRCFILES)
	rm -rf .depend
	$(COMPILER) -MM $^>>./.depend;

$(OBJECTS): %.o: $(SRCDIR)/%

setup:
	mkdir $(SRCBIN)
	mkdir $(OBJDIR)

run: $(OUTPUTFILE)
	./$(OUTPUTFILE)

.PHONY: install
install: compile
	cp $(OUTPUTFILE) /usr/local/bin/calc	

.PHONY: clean
clean:
	rm -rf $(SRCBIN)
	rm -rf $(TESTOUTDIR)
