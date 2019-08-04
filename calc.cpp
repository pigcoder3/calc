#include <cstring>
#include <iostream>
#include <algorithm>
#include <cmath>

bool showSteps = false;
int depth = 0;

void printStep(std::string step);
std::string recreateEquation(std::string equation, int index, int calculationStartIndex, long double newNumber);
long double getNumber(std::string input, int index);
std::string removeZeros(std::string input);

std::string calculate(std::string equation) {

	//PEMDAS

	bool anythingToCalculate = false;
	bool parenthesis = false;
	bool absoluteValue = false;
	bool exponentsOrRoots = false; 
	bool multiplyOrDivide = false;
	bool addOrSubtract = false;
	bool inParenthesis = false;
	bool inAbsoluteValue = false;
	bool scoutingPhase = true;
	std::string nextCalculation;
	int calculationStartIndex = 0;
	bool inCalculation = 0;	
	while(true) {

		if(showSteps && !scoutingPhase) { printStep(equation); }
		
		if(scoutingPhase) { //Reset everything to scout again
			parenthesis = false;
			absoluteValue = false;
			exponentsOrRoots = false;
			multiplyOrDivide = false;
			addOrSubtract = false;
			inParenthesis = false;
			inAbsoluteValue = false;
			calculationStartIndex = 0;
			inCalculation = 0;
		}

		for(int i=0; i<equation.length(); i++) {
		
			if(!inCalculation && !inParenthesis && !inAbsoluteValue && (isdigit(equation[i]) || (equation[i] == '-' && !isdigit(equation[i-1])))) {
				calculationStartIndex = i;
				inCalculation = true;
			}

			if(equation[i] == '(') { //Getting the open parenthesis
				calculationStartIndex = i;
				inParenthesis = true;
				parenthesis = true;
				inCalculation = true;
			} else if(!scoutingPhase && equation[i] == ')' && inParenthesis) { //When closing parenthesis in calculation phase, use recursion to calculate what is inside
				depth++;
				equation = equation.substr(0, calculationStartIndex) + //Beginning
					calculate(equation.substr(calculationStartIndex+1, i-calculationStartIndex-1)) + //Maths
					equation.substr(i+1, equation.length()); //End
				inParenthesis = false;
				break;
			} else if(equation[i] == '|') { //When closing absolute value in calculation phase, use recursion to calculate what is inside.
					
				if(!scoutingPhase) {
					if(inAbsoluteValue) { //The end of the absolute value block
						equation = equation.substr(0, calculationStartIndex) + //Beginning
							removeZeros(std::to_string(fabs(stod(calculate(equation.substr(calculationStartIndex+1, i-1-calculationStartIndex)))))) + //Maths
							equation.substr(i+1, equation.length()); //End
						break;
					} else { //The start
						calculationStartIndex = i;
						inCalculation = true;
						absoluteValue = true;
						inAbsoluteValue = true;
					}
				} else {
					absoluteValue = true;
				}
			} else if(equation[i] == '^' || equation[i] == 'V') { //Dealing with exponents and roots
				if(inParenthesis || inAbsoluteValue) {continue; }
				if((parenthesis && !inParenthesis)) { inCalculation = false;continue;} //Dont use this as part of the equation if there is something higher in PEMDAS
				if(!scoutingPhase && !parenthesis) { //Make sure the other parts of PEMDAS that come first do not exist
					if(equation[i] == '^') {
						//std::cout << "Exponent: " << equation.substr(calculationStartIndex, i) << endl;
						try {
							long double secondNumber = getNumber(equation, i+1);
							long double newNumber = pow(stod(equation.substr(calculationStartIndex, i)), secondNumber);
							if(showSteps && !scoutingPhase) { printStep("Exponent: " + equation.substr(calculationStartIndex, i+1) + removeZeros(std::to_string(secondNumber))); }
							equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
						} catch(std::invalid_argument) {
							std::cout << "Error: invalid syntax. (When using exponents)" << std::endl;
							exit(-1);
						}
					} else if(equation[i] == 'V') {
						//std::cout << "Root: " << equation.substr(calculationStartIndex, i) << endl;
						try {
							long double secondNumber = getNumber(equation, i+1);
							long double newNumber = pow(secondNumber, 1.0/stod(equation.substr(calculationStartIndex, i))); //4^(1/2) = sqrt(4)	
							if(showSteps && !scoutingPhase) { printStep("Root: " + equation.substr(calculationStartIndex, i+1) + removeZeros(std::to_string(secondNumber))); }
							equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
						} catch(std::invalid_argument) {
							std::cout << "Error: invalid syntax. (When using roots)" << std::endl;
							exit(-1);
						}
					}
				} else {
					exponentsOrRoots = true;
				}
			} else if(equation[i] == '*' || equation[i] == '/') { //Make sure the other parts of PEMDAS that come first do not exist
				if(inParenthesis || inAbsoluteValue) {continue; }
				if(((parenthesis && !inParenthesis) || exponentsOrRoots)) { inCalculation = false;continue;} //Dont use this as part of the equation if there is something higher in PEMDAS
				if(!scoutingPhase && !parenthesis && !exponentsOrRoots) {
					if(equation[i] == '*') { //Multiply
						try {
							long double secondNumber = getNumber(equation, i+1);
							long double newNumber = stod(equation.substr(calculationStartIndex, i)) * secondNumber;
							if(showSteps && !scoutingPhase) { printStep("Multiply: " + equation.substr(calculationStartIndex, i+1) + removeZeros(std::to_string(secondNumber))); }
							equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
						} catch(std::invalid_argument) {
							std::cout << "Error: invalid syntax. (During multiplication)" << std::endl;
							exit(-1);
						}
					} else { //Divide
						try {
							long double secondNumber = getNumber(equation, i+1);
							long double newNumber = stod(equation.substr(calculationStartIndex, i)) / secondNumber;
							if(showSteps && !scoutingPhase) { printStep("Divide: " + equation.substr(calculationStartIndex, i+1) + removeZeros(std::to_string(secondNumber))); }
							equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
						} catch(std::invalid_argument) {
							std::cout << "Error: invalid syntax. (During division)" << std::endl;
							exit(-1);
						}
					}
					break;
				} else {
					multiplyOrDivide = true;
				}
			} else if(equation[i] == '+' || (equation[i] == '-' && isdigit(equation[i-1]))) {
				//Special case: The minus sign and the negative sign are the same char. The program checks for a digit to the left. If there is a digit, then this is a minus sign, otherwise it is a negative sign.

				if(inParenthesis || inAbsoluteValue) { continue; }
				if(((parenthesis && !inParenthesis) || exponentsOrRoots || multiplyOrDivide)) { inCalculation = false; continue;} //Dont use this as part of the equation if there is something higher in PEMDAS
				if(!scoutingPhase && !parenthesis && !exponentsOrRoots && !multiplyOrDivide) { //Make sure the other parts of PEMDAS that come first do not exist
					if(equation[i] == '+') { //Add
						try {
							long double secondNumber = getNumber(equation, i+1);
							long double newNumber = stod(equation.substr(calculationStartIndex, i)) + secondNumber;
							if(showSteps && !scoutingPhase) { printStep("Add: " + equation.substr(calculationStartIndex, i+1) + removeZeros(std::to_string(secondNumber))); }
							equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
						} catch(std::invalid_argument) {
							std::cout << "Error: invalid syntax. (During addition)" << std::endl;
							exit(-1);
						}
					} else { //Subtract
						try {
							long double secondNumber = getNumber(equation, i+1);
							long double newNumber = stod(equation.substr(calculationStartIndex, i)) - secondNumber;
							if(showSteps && !scoutingPhase) { printStep("Subtract: " + equation.substr(calculationStartIndex, i+1) + removeZeros(std::to_string(secondNumber))); }
							equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
						} catch(std::invalid_argument) {
							std::cout << "Error: invalid syntax. (During subtraction)" << std::endl;
							exit(-1);
						}
					}

					break;
				} else {
					addOrSubtract = true;
				}
			}
		}
		calculationStartIndex = 0;
		scoutingPhase = !scoutingPhase; //Every other loop will be a scouting phase that checks for specific things (PEMDAS)	
		//Make sure that there is nothing left to calculate
		if(!scoutingPhase && !parenthesis && !absoluteValue && !exponentsOrRoots && !multiplyOrDivide && !addOrSubtract) { depth--; return equation; } //Note the "!scoutingPhase", we just left it
	}
}

void printStep(std::string step) {

	for(int i=0; i<depth; i++) {
		std::cout << "  "; //Print out a spacing
	}
	std::cout << step << std::endl; //Print out the step
}

//recreates the equation with the new calculated number
std::string recreateEquation(std::string equation, int i, int calculationStartIndex, long double newNumber) {
	long double otherNumber = getNumber(equation, i+1); //Gets the second number
	equation = equation.substr(0, calculationStartIndex) + //Adds the beginning
	removeZeros(std::to_string(newNumber)) + //Adds the new number
	equation.substr(i+1 + removeZeros(std::to_string(otherNumber)).length(),
		equation.length()-1 - i+1 + removeZeros(std::to_string(otherNumber)).length()); //Adds the stuff after
	return equation;
}

//Used to get numbers from the equation
long double getNumber(std::string input, int index) {
	std::string number;

	//Add each digit/decimal point to the new string
	for(int i=index; i<input.length(); i++) {
		if((isdigit(input[i]) || input[i] == '-') || (input[i] == '.')) { //The negative sign would be at the front of the number so we are ok
			number+=input[i];
		} else {
			//The number has ended
			break;
		}
	}

	//Convert the string to long double and return it
	try {
		return stod(number);
	} catch (std::invalid_argument) {
		std::cout << "Error: Invalid syntax. (Unkown origin)" << std::endl;
		exit(-1);
	}
}

//Removes all trailing zeros
std::string removeZeros(std::string input) {

	//Note the i>0, we should leave 1 digit in case it is 0
	for(int i=input.length()-1; i>0; i--) {
		if(input[i] == '.') { //We have reached the decimal point, we want to preserve everything in front of it
			input.pop_back();
			break;
		} else if(input[i] == '0') {
			input.pop_back();
		} else {
			break;
		}
	}

	return input;
}

int main(int argc, char **argv) {

	//If the incorrect number of arguments were given, give the usage
	if(argc < 2 || argc > 3) { 
		std::cout << "Usage: calc equation [-s] \n";
		return 0;
	}

	for(int i = 0; i < argc; i++) {
	
		//Send the help message
		if(strncmp(argv[i], "--help", strlen(argv[i])) == 0 || strncmp(argv[i], "-h", strlen(argv[i])) == 0) {
			const char* help = "[HELP]\n"
				"Usage: calc [equation]\n"
				"       calc -h\n"
				"\n"
				"Options:\n"
				"  -h - show this help message.\n"
				"  -s - show steps to solve.\n"
				"\n"
				"Notes:\n"
				"  [1] Quotation marks should be present to make sure that your shells interprets the equation as a single argument.\n"
				"  [2] Calc follows rules of order of operations.\n"
				"  [3] If certain symbols are not placed in their proper positions (or not placed at all), the parser will have incorrect responses (without an error because none will be detected).\n"
				"    Ex: 4*5(3+2) will output 200. This is because the program will place the output of the stuff within the parenthesis back in the equation (So it will read 4*55).\n"
				"      To make this work, do 4*5*(3+2) to get the intended result.\n"
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
				"Examples:\n"
				"  4*5*(3+2) 	= 100 [See note 3]\n"
				"  (-4^2)+2/3 	= 16.6666666\n"
				"  |-5-2| 	= 7\n"
				"  2/(3V8) 	= 1\n";
			std::cout << help;
		} else if (strncmp(argv[i], "-s", strlen(argv[i])) == 0) {
			showSteps = true;
		}
	}
	//Remove all spaces
	std::string str = argv[1];
	str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

	//Begin recursive calculations
	std::cout << calculate(str) << std::endl;

	return 0;
}
