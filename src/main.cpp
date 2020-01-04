/*

	Calc - A free command line calculator

	main.cpp

	MIT License

	Copyright (c) 2019 Sean Johnson

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

//Standard headers
#include <iostream>

//Project Headers
#include "calc.h"

std::string version = "2.2";

bool disableSyntaxCheckWarning = false;
bool enableConsole = true;
int equationArgument = -1; //Stays -1 until found

const char* help = "[HELP]\n"
				"Usage: calc expression [-p] [-s] [-n] [-d] [-c]\n"
				"       calc [-h] [-v]\n"
				"\n"
				"       If no expression is given, and the -p flag is not used, calc opens a console\n"
				"\n"
				"       -p allows you to pipe an expression to calc."
				"\n"
				"Options:\n"
				"  -c - Disable Syntax checks. WARNING: Disabling syntax checking could cause strange\n"
				"       errors during calculation or even give an incorrect result, because syntax\n"
				"       checking stops the program if something is wrong. This should only be used\n"
				"       if there is a bug in the syntax checker. Disable this warning with -w.\n"
				"  -d - Show debug messages.\n"
				"  -h - show this help message.\n"
				"  -p - Make calc listen for an expression through stdin (Allows for piping).\n"
				"  -s - show steps to solve.\n"
				"  -n - show result in scientific notation.\n"
				"  -v --version - show version.\n"
				"  -w - Disable the warning that comes with disabling syntax checks (-c).\n"
				"\n"
				"Console Commands:\n"
				"  help - show this help messages.\n"
				"  debug - toggle debug messages.\n"
				"  steps - toggle the steps for solving.\n"
				"  snotation - toggle scientific notation.\n"
				"  version - display the version.\n"
				"  syntaxcheck - toggle syntax checking.\n"
				"  exit - exit calc.\n"
				"\n"
				"Notes:\n"
				"  [1] Quotation marks should be present to make sure that your shell interprets the\n"
				"      expression as a single argument.\n"
				"  [2] You must place multiplication symbols when using the distributive property.\n"
				"      [See example 1]\n"
				"  [3] Calc follows rules of order of operations.\n"
				"    [3.1] Order of operations does not state when trigonometric functions should be\n"
				"          calculated, so calc does it between exponents and multiplication/division.\n"
				"       [3.1.1] It is strongly advised to use parenthesis around trig functions anyway\n"
				"               to ensure that everything is calculated the way it is intended.\n"
				"\n"
				"Specific Syntax: (Just throw these together like is done with real equations)\n"
				"  Add: +\n"
				"  Subtract: -\n"
				"  Divide: /\n"
				"  Multiply: *\n"
				"  Parenthesis: ( )\n"
				"  Absolute Value: | |\n"
				"  Power: number^power\n"
				"  Root: ((root)V(number)) Note that the parenthesis should be present to ensure that\n"
				"        the parser reads it the correct way. Inner parenthesis are not necessary if\n"
				"        there is only 1 number within.\n"
				"  Trig function: sin(stuff) [See note 3.1]\n"
				"\n"
				"Examples: [See note 1]\n"
				"  4*5*(3+2)        = 100 [See note 2]\n"
				"  (-4^2)+2/3       = 16.6666666\n"
				"  |-5-2|           = 7\n"
				"  2/(3V8)          = 1\n"
				"  sin(1)/sin(1)    = 1\n"
				"\n"
				"Exit status:\n"
				"  -2:   Syntax error\n"
				"  -1:   Invalid flag\n"
				"  0:    Everthing is okay\n";

int main(int argc, char** argv) {

	//Initial arguments
	for(int i = 1; i < argc; i++) { //Skip the first arg because that is the program name
	
		//Send the help message
		if(strncmp(argv[i], "help", strlen("help")) == 0 || strncmp(argv[i], "-h", strlen("-h")) == 0) {
			std::cout << help;
			exit(0);
		} else if (strncmp(argv[i], "-s", strlen(argv[i])) == 0) {
			showSteps = true;
		} else if (strncmp(argv[i], "-n", strlen(argv[i])) == 0) {
			sciNotation = true;
		} else if (strncmp(argv[i], "-d", strlen(argv[i])) == 0) {
			debug = true;
		} else if (strncmp(argv[i], "-c", strlen(argv[i])) == 0) {
			disableSyntaxCheck = true;
		} else if (strncmp(argv[i], "-w", strlen(argv[i])) == 0) {
			disableSyntaxCheckWarning = true;
		} else if (strncmp(argv[i], "-p", strlen(argv[i])) == 0) {
			enableConsole = false;
		} else if ((strncmp(argv[i], "-v", strlen(argv[i])) == 0) || (strncmp(argv[i], "--version", strlen(argv[i])) == 0)) {
			std::cout << "version: " << version << std::endl;
			exit(0);
		} else if (strncmp(argv[i], "-v", strlen(argv[i])) == 0) {
			enableConsole = true;
		} else { //This is either the equation or its an invalid flag
			if(equationArgument != -1) { //-1 if not found yet
				std::cout << "Invalid flag: " << argv[i] << std::endl;
				exit(-1);
			} else {
				enableConsole = false;
				equationArgument = i;
			}
		}
	}

	if(disableSyntaxCheck && !disableSyntaxCheckWarning) { //Give a warning if the user did not disable it using the -w flag
		std::cout << "WARNING: You have disabled syntax checking, which is strongly ill-advised." << std::endl;
		std::cout << "         Disabling syntax checking could cause strange errors or output" << std::endl;
		std::cout << "         an incorrect result. Only use this if you are absolutely sure that" << std::endl;
		std::cout << "         you have perfect syntax." << std::endl;
	}

	if(!enableConsole) {
		int length = 0;
		if(equationArgument != -1) { //Equation in arguments
			std::string expression = argv[equationArgument];
			length = parse(expression);
		} else { //Equation not in arguments, so get from stdin. Can be piped this way
			std::string buffer;
			getline(std::cin, buffer);
			length = parse(buffer);
		}

		if(length == 0) { exit(-2); }
	
		//Begin recursive calculations
		calculate(NULL, 0, length, false);
		if(sciNotation) { //Print the result in scientific notation
			std::cout << std::scientific;
		}

		std::cout << list->root->value << std::endl;
	} else {
		int length = 0;
		while(true) {
			//Get commands
			list->clean();
			std::string input;
			std::cout << " > ";
			getline(std::cin, input);
			//Send the help message
			if(input.compare("help") == 0) {
				std::cout << help;
				continue;
			} else if (input.compare("steps") == 0) {
				showSteps = !showSteps;
				if(showSteps)
					std::cout << "Steps to solve enabled" << std::endl;
				else
					std::cout << "Steps to solve disabled" << std::endl;
				continue;
			} else if (input.compare("debug") == 0) {
				debug = !debug;
				if(debug)
					std::cout << "Debug messages enabled" << std::endl;
				else
					std::cout << "Debug messages disabled" << std::endl;
				continue;
			} else if (input.compare("snotation") == 0) {
				sciNotation = !sciNotation;
				if(sciNotation)
					std::cout << "Scientific notation output enabled" << std::endl;
				else
					std::cout << "Scientific notation output disabled" << std::endl;
				continue;
			} else if (input.compare("version") == 0) {
				std::cout << "Version: " << version << std::endl;
				continue;
			} else if (input.compare("syntaxcheck") == 0) {
				disableSyntaxCheck = !disableSyntaxCheck;
				if(disableSyntaxCheck) {
					std::cout << "Syntax checking disabled" << std::endl;
					std::cout << "WARNING: You have disabled syntax checking, which is strongly ill-advised." << std::endl;
					std::cout << "         Disabling syntax checking could cause strange errors or output" << std::endl;
					std::cout << "         an incorrect result. Only use this if you are absolutely sure that" << std::endl;
					std::cout << "         you have perfect syntax." << std::endl;
				} else {
					std::cout << "Syntax checking enabled" << std::endl;
				}
				continue;
			} else if (input.compare("exit") == 0) {
				exit(0);	
			} //Otherwise, we have an equation (Or something wacky)
		
			length = parse(input);

			if(length == 0) { continue; } //There was a syntax error or nothing was inputted
	
			calculate(NULL, 0, length, false);
			if(sciNotation) { //Print the result in scientific notation
				std::cout << std::scientific;
			}
	
			std::cout << list->root->value << std::endl;
		}
	}

	return 0;

}
