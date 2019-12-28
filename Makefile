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
#     - obj/
#   - include/ (Libraries)
#   - src/ (Project sources)
#   - tests/ (Project tests, I hope you are using these)
#     - out (Generated)
#       - obj (Generated)

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
TESTSRCFILES = $(shell find $(TESTDIR) -name "*.cpp")
TESTLIBS = -lgtest -lgtest_main
TESTOUTDIR = ./tests/out
TESTSWITHCORRECTSUFFIX = $(subst .cpp,.test,$(TESTSRCFILES)) #Figure out how to change prefix and suffix
TESTOUTFILES = $(subst $(TESTDIR),$(TESTOUTDIR),$(TESTSWITHCORRECTSUFFIX))
TESTOPTIONS = -std=c++11

#objects
OBJDIR = ./bin/obj
OBJECTSWITHCORRECTSUFFIX = $(subst .cpp,.o,$(SRCFILES)) #Figure out how to change prefix and suffix
OBJECTS = $(subst $(SRCDIR),$(OBJDIR),$(OBJECTSWITHCORRECTSUFFIX))

TESTOBJDIR = ./tests/out/obj
TESTOBJECTSWITHCORRECTSUFFIX = $(subst .cpp,.o,$(SRCFILES)) #Figure out how to change prefix and suffix
TESTOBJECTS = $(subst $(SRCDIR),$(OBJDIR),$(OBJECTSWITHCORRECTSUFFIX))

.PHONY: compile
compile: $(OUTPUTFILE)

.PHONY: all
all: $(OUTPUTFILE) test

$(OUTPUTFILE): $(SRCBIN) $(OBJECTS)
	#Linking	
	$(COMPILER) $(OBJECTS) -o $(OUTPUTFILE) $(SRCLIBS) $(SRCOPTIONS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRCBIN) $(OBJDIR)
	#Create object	
	$(COMPILER) -c $< -o $@ $(SRCLIBS) $(SRCOPTIONS)


test: $(TESTOUTDIR) $(TESTOBJDIR) $(TESTOBJECTS) $(TESTOUTFILES)

$(TESTOUTDIR)/%.test: $(TESTOBJDIR)/%.o $(OBJDIR)/%.o
	#Link the objects
	$(COMPILER) $^ -o $@ $(TESTLIBS) $(TESTOPTIONS)
	#Run the new executable
	./$@

$(TESTOBJDIR)/%.o: $(TESTDIR)/%.cpp
	#Compile the test (Create its object file) with its respective object file
	$(COMPILER) -c $< -o $@ $(TESTOPTIONS)

#Generated project structure directories	
$(SRCBIN):
	mkdir $(SRCBIN)

$(OBJDIR): $(SRCBIN)
	mkdir $(OBJDIR)

$(TESTOUTDIR):
	mkdir $(TESTOUTDIR)

$(TESTOBJDIR): $(TESTOUTDIR)
	mkdir $(TESTOBJDIR)

.PHONY: install #You're gonna have to change this one to your needs
install: $(OUTPUTFILE)
	cp $(OUTPUTFILE) /usr/local/bin/$(PROJECT)

.PHONY: clean
clean:
	rm -rf $(SRCBIN)
	rm -rf $(TESTOUTDIR)
