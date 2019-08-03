#include <cstring>
#include <iostream>
#include <algorithm>
#include <cmath>

std::string recreateEquation(std::string equation, int index, int calculationStartIndex, long double newNumber);
long double getNumber(std::string input, int index);
std::string removeZeros(std::string input);

std::string calculate(std::string equation) {

	//std::cout << "Calculating: " << equation << std::endl;

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

		//std::cout << equation << std::endl;
		
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
						long double newNumber = pow(stod(equation.substr(calculationStartIndex, i)), getNumber(equation, i+1));
						equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
					} else if(equation[i] == 'V') {
						//std::cout << "Root: " << equation.substr(calculationStartIndex, i) << endl;
						long double newNumber = pow(getNumber(equation, i+1), 1.0/stod(equation.substr(calculationStartIndex, i))); //4^(1/2) = sqrt(4)	
						equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
					}
				} else {
					exponentsOrRoots = true;
				}
			} else if(equation[i] == '*' || equation[i] == '/') { //Make sure the other parts of PEMDAS that come first do not exist
				if(inParenthesis || inAbsoluteValue) {continue; }
				if(((parenthesis && !inParenthesis) || exponentsOrRoots)) { inCalculation = false;continue;} //Dont use this as part of the equation if there is something higher in PEMDAS
				if(!scoutingPhase && !parenthesis && !exponentsOrRoots) {
					if(equation[i] == '*') { //Multiply
						//std::cout << "Multiply: " << equation.substr(calculationStartIndex, i) << endl;
						long double newNumber = stod(equation.substr(calculationStartIndex, i)) * getNumber(equation, i+1);
						equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
					} else { //Divide
						//std::cout << "Divide: " << equation.substr(calculationStartIndex, i) << endl;
						long double newNumber = stod(equation.substr(calculationStartIndex, i)) / getNumber(equation, i+1);
						equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
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
						//std::cout << "Add: " << equation.substr(calculationStartIndex, i) << std::endl;
						long double newNumber = stod(equation.substr(calculationStartIndex, i)) + getNumber(equation, i+1);
						equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
					} else { //Subtract
						//std::cout << "Subtract: " << equation.substr(calculationStartIndex, i) << std::endl;
						long double newNumber = stod(equation.substr(calculationStartIndex, i)) - getNumber(equation, i+1);
						equation = recreateEquation(equation, i, calculationStartIndex, newNumber);
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
		if(!scoutingPhase && !parenthesis && !absoluteValue && !exponentsOrRoots && !multiplyOrDivide && !addOrSubtract) { return equation; } //Note the "!scoutingPhase", we just left it
	}
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
	return stod(number);
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
	if(argc != 2) { 
		std::cout << "Usage: calc equation\n";
		return 0;
	}

	//Send the help message
	if(strncmp(argv[1], "--help", strlen(argv[1])) == 0 || strncmp(argv[1], "-h", strlen(argv[1])) == 0) {
		const char* help = "[HELP]\n"
			"Usage: calc [equation]\n"
			"       calc -h\n"
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
	} else {
		//Remove all spaces
		std::string str = argv[1];
		str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

		//Begin recursive calculations
		std::cout << calculate(str) << std::endl;
	}
}
