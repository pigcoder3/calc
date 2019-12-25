//Standard headers
#include <iostream>

//Project Headers
#include "calc.h"

int main(int argc, char** argv) {

	//If the incorrect number of arguments were given, give the usage
	if(argc < 2 || argc > 5) { 
		std::cout << "Usage: calc equation [-s] [-n] [-h] [-d]\n";
		return 0;
	}

	for(int i = 0; i < argc; i++) {
	
		//Send the help message
		if(strncmp(argv[i], "--help", strlen("--help")) == 0 || strncmp(argv[i], "-h", strlen("-h")) == 0) {
			const char* help = "[HELP]\n"
				"Usage: calc equation [-s] [-n] [-d]\n"
				"       calc -h\n"
				"\n"
				"Options:\n"
				"  -h - show this help message.\n"
				"  -s - show steps to solve.\n"
				"  -n - show result in scientific notation.\n"
				"  -d - Show debug messages.\n"
				"\n"
				"Notes:\n"
				"  [1] Quotation marks should be present to make sure that your shell interprets the equation as a single argument.\n"
				"  [2] Calc follows rules of order of operations.\n"
				"\n"
				"Specific Syntax: (Just throw these together like is done with real equations)\n"
				"  Add: +\n"
				"  Subtract: -\n"
				"  Divide: /\n"
				"  Multiply: *\n"
				"  Parenthesis: ( )\n"
				"  Absolute Value: | |\n"
				"  Power: number^power\n"
				"  Root: ((root)V(number)) Note that the parenthesis should be present to ensure that the parser reads it the correct way. Inner parenthesis are not necessary if there is only 1 number within.\n"
				"\n"
				"Examples: [See note 1]\n"
				"  4*5*(3+2) 	= 100 [See note 3]\n"
				"  (-4^2)+2/3 	= 16.6666666\n"
				"  |-5-2| 	= 7\n"
				"  2/(3V8) 	= 1\n";
			std::cout << help << std::endl;
			exit(0);
		} else if (strncmp(argv[i], "-s", strlen(argv[i])) == 0) {
			showSteps = true;
		} else if (strncmp(argv[i], "-n", strlen(argv[i])) == 0) {
			sciNotation = true;
		} else if (strncmp(argv[i], "-d", strlen(argv[i])) == 0) {
			debug = true;
		}
	}

	int length = parse(argv[1]);

	//Begin recursive calculations

	calculate(NULL, 0, length, false);
	if(sciNotation) { //Print the result in scientific notation
		std::cout << std::scientific;
	}

	std::cout << root->value << std::endl;

	return 0;

}
