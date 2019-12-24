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

all: clean compile test

compile: clean
	#Create needed directories
	mkdir $(SRCBIN)
	#Compile it
	$(COMPILER) $(SRCFILES) -o $(OUTPUTFILE) $(SRCLIBS) $(SRCOPTIONS)
	#Done compiling

test:
	#Create needed directories
	mkdir $(TESTOUTDIR)
	#Compile (I need to compile the sources with the tests)
	$(COMPILER) $(TESTSRCFILES) -o $(TESTOUTFILE) $(TESTLIBS) $(TESTOPTIONS)

	#Run
	./$(TESTOUTFILE)

run: $(OUTPUTFILE)
	./$(OUTPUTFILE)

.PHONY: install
install: compile
	cp $(OUTPUTFILE) /usr/local/bin/calc	

.PHONY: clean
clean:
	rm -rf $(SRCBIN)
	rm -rf $(TESTOUTDIR)
