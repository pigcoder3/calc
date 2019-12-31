/*

	Calc - A free command line calculator

	Calc.cpp


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

#include <cstring>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "calc.h"

bool showSteps = false;
bool sciNotation = false;
bool debug = false;
bool disableSyntaxCheck = false;
int depth = 0;

LinkedList *list = new LinkedList();

//types:
// 0: regular number
// 1: operator (will be stored as number)

//operator values:
// 0: +
// 1: -
// 2: *
// 3: /
// 4: V
// 5: ^
// 6: (
// 7: )
// 8: |

int lastNumberGottenLength = 0;

//std::string recreateEquation(std::string equation, int index, int calculationStartIndex, long double newNumber);
long double getNumberAsNumber(std::string input, int index);
std::string getNumberAsString(std::string input, int index);
std::string removeZeros(std::string input);
/*
void LinkedList::replace_nodes(struct node *start_node, int beforeIndex, int startIndex, int afterIndex, long double result, int length);
void LinkedList::removeNode(struct node *node);
void LinkedList::insertNode(int index, struct node* newNode);
struct node* LinkedList::jumpTo(int i);
void LinkedList::display();
int LinkedList::length();*/
std::string removeZeros(std::string input);
std::string getSymbol(int value);
void printStep(std::string operation, struct node* begin, int length);

void calculate(struct node *sub_root_last, int startIndex, int length, bool absolute_value) {

	if(debug) {
		std::cout << "In calculation" << std::endl;
		std::cout << "depth: " << depth << std::endl;
	}

	if(depth > 0) { //Remove any paarenthesis or absolute value symbols to prevent infinite recursion
		//std::cout << "Removing parenthesis and absolute value symbols" << std::endl;

		list->removeNode(sub_root_last->next);
		length--;
		//Get to the thing
		struct node *current = sub_root_last;

		int i=0;
		while(current && i<length) {
			current = current->next;
			i++;
		}

		list->removeNode(current); //remove it (This is much more optimized)
		length--;
	}

	//PEMDAS
	//Note that trig functions go between e and m

	bool anythingToCalculate = false;
	bool parenthesis = false;
	bool absoluteValue = false;
	bool exponentsOrRoots = false; 
	bool trigFunctions = false;
	bool multiplyOrDivide = false;
	bool addOrSubtract = false;
	bool inParenthesis = false;
	bool inAbsoluteValue = false;
	bool scoutingPhase = true;
	struct node *last_node = sub_root_last;	
	struct node *calculation_start_node_last = sub_root_last;
	int calculationStartIndex = 0;
	bool inCalculation = 0;	
	long double result;
	while(true) {

		if(showSteps && !scoutingPhase) { 
			if(sub_root_last)
				printStep("", sub_root_last->next, length+2);
			else
				printStep("", list->root, length+2);
		}
		
		if(scoutingPhase) { //Reset everything to scout again
			parenthesis = false;
			absoluteValue = false;
			exponentsOrRoots = false;
			trigFunctions = false;
			multiplyOrDivide = false;
			addOrSubtract = false;
			inParenthesis = false;
			inAbsoluteValue = false;
			calculationStartIndex = 0;
			inCalculation = 0;
		}

		struct node *current;
		if(sub_root_last) {
			current = sub_root_last->next; //this has to be a thing because if it wasnt, then we wouldnt be in this iteration
		} else {
			current = list->root;
		}
		int i=0;

		while(current != NULL && i < length) {

			int type = current->type;
			long double value = current->value;

			if(!inCalculation && !inParenthesis && !inAbsoluteValue && (type == 0 || (type == 1 && ( value == 9 || value == 10 || value == 11)))) {
				calculationStartIndex = i;
				if(debug) { std::cout << "Beginning to read a calculation (Number found): " << value << std::endl; }
				calculation_start_node_last = last_node;
				inCalculation = true;
			}

			if(type == 1) {
				if(value == 6) { //Getting the open parenthesis
					calculationStartIndex = i;
					calculation_start_node_last = last_node;
					inParenthesis = true;
					parenthesis = true;
					inCalculation = true;
				} else if(!scoutingPhase && value == 7 && inParenthesis) { //When closing parenthesis in calculation phase, use recursion to calculate what is inside
					depth++;
					calculate(calculation_start_node_last, startIndex+calculationStartIndex, i-calculationStartIndex+1, false);
					length-=i-calculationStartIndex;
						
					inParenthesis = false;
					break;
				} else if(value == 8) { //When closing absolute value in calculation phase, use recursion to calculate what is inside.
					if(!scoutingPhase) {
						if(inAbsoluteValue) { //The end of the absolute value block
							depth++;	
							calculate(calculation_start_node_last, startIndex+calculationStartIndex, i-calculationStartIndex, true);
							length-=i-calculationStartIndex;
							inAbsoluteValue = false;
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
				} else if(value == 5 || value == 4) { //Dealing with exponents and roots
					if(scoutingPhase) {
						exponentsOrRoots = true;
					}
					if(inParenthesis || inAbsoluteValue) {
						i++; //for loop index increase
						last_node = current;
						current = current->next;	
						continue; 
					}
					if((parenthesis && !inParenthesis)) { 
						i++; //for loop index increase
						last_node = current;
						current = current->next;
						inCalculation = false;
						continue;
					} //Dont use this as part of the experssion if there is something higher in PEMDAS
					if(!scoutingPhase && !parenthesis) { //Make sure the other parts of PEMDAS that come first do not exist
						if(value == 5) {
							
							if((showSteps || debug) && !scoutingPhase) { printStep("Exponent: ", last_node, 3); }
							try {
								result = pow(last_node->value, current->next->value);
								list->replace_nodes(last_node, calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
								length-=2;
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (When using exponents)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (When using exponents)" << std::endl;
								exit(-1);
							}
	
						} else if(value == 4) {
							
							if((showSteps || debug) && !scoutingPhase) { printStep("Root: ", last_node, 3); }
							try {
								result = pow(current->next->value, 1.0/(last_node->value));
								list->replace_nodes(last_node, calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
								length-=2;
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (When using roots)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (When using roots)" << std::endl;
								exit(-1);
							}
	
						}
						break;
					}
				} else if(value == 9 || value == 10 || value == 11) { //Trigonometric functions
					if(scoutingPhase) {
						trigFunctions = true;
					}

					if(inParenthesis || inAbsoluteValue) { 
						i++; //for loop index increase
						last_node = current;
						current = current->next;
						continue; 
					} if(((parenthesis && !inParenthesis) || (absoluteValue && !inAbsoluteValue) || exponentsOrRoots)) {
						i++; //for loop index increase
						current = current->next;
						last_node = current;
						inCalculation = false;
						continue;
					} //Dont use this as part of the expressionif there is something higher in PEMDAS
					else if(!scoutingPhase && !parenthesis && !exponentsOrRoots) {
						if(value == 9) { //Sin
							if((showSteps || debug) && !scoutingPhase) { printStep("Sine: ", current, 2); }
							try {
								result = sin(current->next->value);
								list->replace_nodes(current, i, i+startIndex, startIndex+i+2, result, 2);
								length--;	
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During Sine)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During Sine)" << std::endl;
								exit(-1);
							}

						} else if(value == 10) { //Cos
							if((showSteps || debug) && !scoutingPhase) { printStep("Cosine: ", current, 2); }
							try {
								result = cos(current->next->value);
								list->replace_nodes(current, i, i+startIndex, startIndex+i+2, result, 2);
								length--;	
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During Cosine)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During Cosine)" << std::endl;
								exit(-1);
							}

						} else if(value == 11) { //Tan
							if((showSteps || debug) && !scoutingPhase) { printStep("Tangent: ", current, 2); }
							try {
								result = tan(current->next->value);
								list->replace_nodes(current, i, i+startIndex, startIndex+i+2, result, 2);
								length-=2;	
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During Tangent)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During Tangent)" << std::endl;
								exit(-1);
							}

						}
						break;
					}
				} else if(value == 2 || value == 3) { //Make sure the other parts of PEMDAS that come first do not exist
					if(scoutingPhase) {
						multiplyOrDivide = true;
					}

					if(inParenthesis || inAbsoluteValue) { 
						i++; //for loop index increase
						last_node = current;
						current = current->next;
						continue; 
					} 
					
					if(((parenthesis && !inParenthesis) || (absoluteValue && !inAbsoluteValue) || exponentsOrRoots || trigFunctions)) {
						i++; //for loop index increase
						current = current->next;
						last_node = current;
						inCalculation = false;
						continue;
					} //Dont use this as part of the expression if there is something higher in PEMDAS
					else if(!scoutingPhase && !parenthesis && !exponentsOrRoots) {
						if(value == 2) { //Multiply
							
							if((showSteps || debug) && !scoutingPhase) { printStep("Multiplication: ", last_node, 3); }
							try {
								result = last_node->value * current->next->value;
								list->replace_nodes(last_node, calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
								length-=2;
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During multiplication)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During multiplication)" << std::endl;
								exit(-1);
							}
						} else if(value == 3) { //Divide
							if((showSteps || debug) && !scoutingPhase) { printStep("Division: ", last_node, 3); }
							try {
								result = last_node->value / current->next->value;
								list->replace_nodes(last_node, calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
								length-=2;	
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During division)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During division)" << std::endl;
								exit(-1);
							}
						}
						break;
					}
				} else if(value == 0 || value == 1) {
					if(scoutingPhase) { 
						addOrSubtract = true;
					}

					if(inParenthesis || inAbsoluteValue) { 
						i++; //for loop index increase
						last_node = current;
						current = current->next;
						continue; 
					}
					if(((parenthesis && !inParenthesis) || (absoluteValue && !inAbsoluteValue) || exponentsOrRoots || trigFunctions || multiplyOrDivide )) {
						i++; //for loop index increase
						last_node = current;
						current = current->next;
						inCalculation = false; 
						continue;
					} //Dont use this as part of the expressionif there is something higher in PEMDAS
					if(!scoutingPhase && !parenthesis && !exponentsOrRoots && !trigFunctions && !multiplyOrDivide) { //Make sure the other parts of PEMDAS that come first do not exist
						if(value == 0) { //Add
							if((showSteps || debug) && !scoutingPhase) { printStep("Addition: ", last_node, 3); }
							try {
								result = last_node->value + current->next->value;
								list->replace_nodes(last_node, calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
								length-=2;	
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During addition)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During addition)" << std::endl;
								exit(-1);
							}
						} else { //Subtract
							if((showSteps || debug) && !scoutingPhase) { printStep("Subtract: ", last_node, 3); }
							try {
								result = last_node->value - current->next->value;
								list->replace_nodes(last_node, calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
								length-=2;
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During subtraction)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During subtraction)" << std::endl;
								exit(-1);
							}
						}
						break;	
					}
				}
			}
			i++; //for loop index increase
			last_node = current;
			current = current->next;
		}
		calculationStartIndex = 0;
		scoutingPhase = !scoutingPhase; //Every other loop will be a scouting phase that checks for specific things (PEMDAS)
		//Make sure that there is nothing left to calculate
		if(!scoutingPhase && !parenthesis && !absoluteValue && !exponentsOrRoots && !trigFunctions && !multiplyOrDivide && !addOrSubtract) { 
			depth--; 

			//There is no need to replace any more nodes because only 1 remains.

			//Do absolute value if this is an absolulte value block
			if(absolute_value) {
				list->jumpTo(startIndex)->value = abs(result);
			}

			if(debug) { std::cout << "Done with this calculation" << std::endl; }

			return;

		} //Note the "!scoutingPhase", we just left it
	}
}

void LinkedList::replace_nodes(struct node *start_node, int beforeIndex, int startIndex, int afterIndex, long double result, int length) {
	if(debug) { std::cout << "replacing nodes " << "LENGTH: " << length << std::endl; }
	//Place the result in the linked list

	struct node *current = start_node;
	struct node *temp;

	//Figure out how to optimize this
	int i=0;
	while(current && i<length) {
		temp = current->next;
		list->removeNode(current);
		current = temp;
		i++;
	}

	list->insertNode(startIndex, create_node(0, result));

	if(debug) { list->display(); }

}

void LinkedList::removeNode(struct node *node) {
	if(debug) { list->display(); std::cout << "removing node: " << node->value << std::endl; }

	//struct node *current = list->root;
	//struct node *last = (struct node*)malloc(sizeof(struct node));

	if(!node->previous) { //If it is the root
		if(debug) { std::cout << "The removed node is at the front" << std::endl; }
		if(node->next != NULL) {
			list->root = root->next;
			node->next = NULL;
  			delete node;
			node = NULL;

			list->root->previous = NULL;
		} else { //This is the only thing in the list
			delete list->root;
			list->root = NULL; //Create a new root so that this can still be used

		}
	} else {
		
		if(!node->next) { //It is at the end
			if(debug) { std::cout << "The removed node is at the end" << std::endl; }
			node->previous->next = NULL;
			delete node;
			node = NULL;
		} else {
			if(debug) { std::cout << "The removed node is somewhere inside the list" << std::endl; }
			node->next->previous = node->previous;
			node->previous->next = node->next;
  			delete node;
			node = NULL;
		}
	}

	node = NULL;

	list->length--;

	if(debug) { list->display(); }

}

//Optimize this?
void LinkedList::insertNode(int index, struct node* newNode) {
	
	if(debug) {
		std::cout << "inserting node: " << index << std::endl;	
		std::cout << "new node value: " << newNode->value << " Index: " << index << std::endl;
	}

	struct node *current = list->root;
	struct node *last;

	int i=0;

	while(current && i < index) { //Get to the right place
		i++;
		last = current;
		current = current->next;	
	}

	if(i == 0) {
		if(debug) { std::cout << "The added node will be at the front" << std::endl; }
		if(list->root) {
			newNode->next = list->root; 
			list->root->previous = newNode;
			list->root = newNode;
			newNode->previous = NULL;
		} else {
			//std::cout << "BRUH" << std::endl;
			list->root = newNode;
		}
	} else {
		if(debug) { std::cout << "The added node will be somewhere inside the list" << std::endl; }
		if(i < list->length) { //Not at the end. +1 is there to ensure that it is supposed to be put at the end and not the 2nd to last index
			if(debug) { std::cout << "Not at the end" << std::endl; }
			newNode->next = current; //add it
			current->previous = newNode;
		} else {
			if(debug) { std::cout << "At the end" << std::endl;}
			newNode->next = NULL;
		}
		newNode->previous = last;
		last->next = newNode;
	}

	list->length++;

	if(debug) { list->display(); }
	
}

/*
//I dont think I need this anymore
int LinkedList::length() {
	
	struct node *current = list->root;

	int len = 0;

	while(current != NULL) {
		current = current->next;
		len++;
	}

	return len;

}*/

void LinkedList::display() {

	struct node *current = list->root;

	if(current) { std::cout << list->length << std::endl; }

	int i=0;
	while(current && i < list->length) {
		std::cout << current->value << ", ";
		current = current->next;
	}

	std::cout << "LENGTH: " << list->length << std::endl;

}

struct node* LinkedList::jumpTo(int i) {
	
	struct node *current = list->root;

	if(debug) { std::cout << "Jumping to index: " << i << std::endl; }

	int index=0;
	while(current != NULL && index < i) {
		current = current->next;
		index++;
	}

	return current;

}

//Free the entire list
void LinkedList::clean() {


	struct node *current = list->root;
	struct node *temp;

	int i=0;
	while(current) {
		temp=current->next;
  		delete current; //I always get errors here (Used to?)
		current = temp;
		list->length--;
	}

	list->root = NULL;

	list->length = 0;

}

void printStep(std::string operation, struct node* begin, int length) {

	for(int i=0; i<depth; i++) {
		std::cout << "  "; //Print out spacing
	}

	std::string output;

	output+=operation;

	//build the expressionfrom the linked list
	struct node *current = begin;
	int i = 0;
	while(current != NULL && i < length) {
		if(current->type == 0) {
			output+=removeZeros(std::to_string(current->value));
		} else {
			output+=getSymbol(current->value);
		}
		current = current->next;
		i++;
	}

	std::cout << output << std::endl;
}

//Used to get numbers from the equation
long double getNumberAsNumber(std::string input, int index) {
	std::string number;

	//Add each digit/decimal point to the new string
	for(int i=index; i<input.length(); i++) {
		if((isdigit(input[i]) || (input[i] == '-' && number.length() == 0)) || (input[i] == '.')) { //The negative sign would be at the front of the number so we are ok
			number+=input[i];
		} else {
			//The number has ended
			break;
		}
	}

	lastNumberGottenLength = number.length();

	//Convert the string to long double and return it
	try {
		return stod(number);
	} catch (std::invalid_argument) {
		std::cout << "Error: Invalid number. (Unkown origin)" << std::endl;
		exit(-1);
	} catch(std::out_of_range) {
		std::cout << "Error: Number too large. (Unknown origin)" << std::endl;
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
		} else if(input[i] == '0' && input.length() > 1) {
			input.pop_back();
		} else {
			break;
		}
	}

	//Remove leading zeros
	for(int i=0; i<input.length(); i++) {
		if(input[i] == '.') {
			break;
		} else if(input[i] == '-') {
			//We dont want to remove the negative sign so do nothing
		} else if(input[i] == '0' && input.length() > 1) {
			input.erase(0, 1);
			i--;
		} else {
			break;
		}
	}

	return input;
}

std::string getSymbol(int value) {
	switch(value) {
		case 0:
			return "+";
		case 1:
			return "-";
		case 2:
			return "*";
		case 3:
			return "/";
		case 4:
			return "V";
		case 5:
			return "^";
		case 6:
			return "(";
		case 7:
			return ")";
		case 8:
			return "|";
		case 9:
			return "sin";
		case 10:
			return "cos";
		case 11:
			return "tan";
		default:
			return "?";
	}
}

std::string error_call(struct node *current) {

	std::string output = "";

	//Prints the current section
	if(current->type == 1) {
		output+=getSymbol(current->value);
	} else {
		output+=removeZeros(std::to_string(current->value));
	}

	//Prints the next section
	if(current->next != NULL) {
		if(current->next->type == 1) {
			output+=getSymbol(current->next->value);
		} else {
			output+=removeZeros(std::to_string(current->next->value));
		}
	}

	return output;
}

struct node* create_node(int type, long double value) {

	struct node *node = new struct node;
	node->type = type; //Set its values
	node->value = value;
	node->next = NULL;
	node->previous = NULL;
	return node;

}

struct node* parse_add_node(bool atFront, struct node* current, struct node* newNode) {

	if(list->root) {	
		current->next = newNode;
		newNode->previous = current;
	} else {
		list->root = newNode;
	}
	current = newNode;
	return current; //For some reason setting the pointer wont set the actual thing (I thought thats how it worked)

}

//Parse the entire expression into a linked list
int parse(char *expression) {
	
	//Remove all spaces
	std::string str = expression;
	str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

	depth = 0;
	list->clean(); //Reset the thing (Just for convenience for tests)
	struct node *current = list->root;

	bool error = false;

	//Turn the equation into a linked list
	for(int i=0; i<str.length(); i++) {
		bool atFront = false;
		if(i==0) 
			atFront = true;
		char c = str[i];
		//Trig functions
		if((i < str.length()-3) && str[i] == 's' && str[i+1] == 'i' && str[i+2] == 'n') {
			current = parse_add_node(atFront, current, create_node(1,9));
			i+=2; //Compensate for the extra characters
		} else if((i < str.length()-3) && str[i] == 'c' && str[i+1] == 'o' && str[i+2] == 's') {
			current = parse_add_node(atFront, current, create_node(1,10));
			i+=2; //Compensate for the extra characters
		} else if((i < str.length()-3) && str[i] == 't' && str[i+1] == 'a' && str[i+2] == 'n') {
			current = parse_add_node(atFront, current, create_node(1,11));
			i+=2; //Compensate for the extra characters
		}

		//Enclosing symbols
		else if(c == '(') {
			current = parse_add_node(atFront, current, create_node(1,6));
		} else if(c == ')') {
			current = parse_add_node(atFront, current, create_node(1,7));
		} else if(c == '|') {
			current = parse_add_node(atFront, current, create_node(1,8));
		}

		//Basic symbols
		else if(c == 'V') {
			current = parse_add_node(atFront, current, create_node(1,4));
		} else if(c == '^') {
			current = parse_add_node(atFront, current, create_node(1,5));
		} else if(c == '/') {
			current = parse_add_node(atFront, current, create_node(1,3));
		} else if(c == '*') {
			current = parse_add_node(atFront, current, create_node(1,2));
		} else if(c == '+') {
			current = parse_add_node(atFront, current, create_node(1,0));
		} else if(c == '-') {
			if (
			(i>0 && (isdigit(str[i-1]) || str[i-1] == ')' || str[i-1] == '|'))
			|| (!isdigit(str[i+1]) && !(str[i+1] == '.' && isdigit(str[i+2])) && !(str[i+1] == '-' && str[i+2] == '.' && isdigit(str[i+3])))
			|| (str[i+1] == '(' || str[i+1] == '|')
			) { //No number so this is is a negative
				current = parse_add_node(atFront, current, create_node(1,1));
			} else { //This is a number
				current = parse_add_node(atFront, current, create_node(0, getNumberAsNumber(str, i)));
				i+=lastNumberGottenLength-1;
			}
		} else if(isdigit(str[i]) || str[i] == '.') { //This is a number
			current = parse_add_node(atFront, current, create_node(0, getNumberAsNumber(str, i)));
			i+=lastNumberGottenLength-1;
		} else { //Unknown symbol (We'll just leave it at -1 so that we can deal with it during syntax error checking
			current = parse_add_node(atFront, current, create_node(-1, 0));
		}

		list->length++;

	}

	if(debug) { std::cout << "New parsed equation: "; list->display(); std::cout << std::endl; }

	if(disableSyntaxCheck) { return list->length; } //All there is left to do here is syntax checking

	//Check for syntax errors
	bool inAbsoluteValue = false;
	int parenthesisDepth = 0;
	current = list->root;
	int i = 0;
	while(current != NULL) {
		int type = current->type;
		long double value = current->value;
		int nextType;
		long double nextValue;
		if(current->next) {
			nextType = current->next->type;
			nextValue = current->next->value;
		}
		
		if(type == -1) { //Unknown symbol
			std::cout << "Syntax error(section: " << i+1 << ") (Unknown Symbol): " << str[i] << std::endl;
			error = true;
		} else if(type == 1) {
			if(value == 9 || value == 10 || value == 11) { //Trig functions
				if(!current->next || (current->next && (current->next->type == 1 && current->next->value != 6))) { //We dont have a number next (Or opening parenthesis)
					std::cout << "Syntax error(section: " << i+1 << ") (No following number/expression): " << error_call(current) << std::endl;
					error = true;
				}
			}
			if(value == 6) { //Opening parenthesis
				if(current->next && current->next->type == 1 && (current->next->value != 8 && current->next->value != 7)) { //Only numbers can follow (Or absolute value or opening parenthesis)
					std::cout << "Syntax error(section: " << i+1 << ") (No preceeding number): " << error_call(current) << std::endl;
					error = true;
				}
				if(current->next && current->next->type == 1 && current->next->value != 6) {
					std::cout << "Syntax error(section: " << i+1 << ") (Empty Parenthesis block): " << error_call(current) << std::endl;
					error = true;
				}
				parenthesisDepth++;
			} else if(value == 7) { //Closing parenthesis
				if(current->next && current->next->type == 0) {
					std::cout << "Syntax error(section: " << i+1 << ") (Unexpected number): " << error_call(current) << " (If you were attempting to multiply them together, just throw in a multiplication symbol (*))" << std::endl;
					error = true;	
				} if(current->next && current->next->type == 1 && (nextValue == 9 || nextValue == 10 || nextValue == 11)) {
					std::cout << "Syntax error(section: " << i+1 << ") (Unexpected trig function): " << error_call(current) << " (If you were attempting to multiply them together, just throw in a multiplication symbol (*))" << std::endl;
					error = true;	
				} if(parenthesisDepth == 0) {
					std::cout << "Syntax error(section: " << i+1 << ") (Extra parenthesis): " << error_call(current) << std::endl;
					error = true;
				} else {
					parenthesisDepth--;
				}
			} else if(value == 8) { //Absolute Value
				if(!inAbsoluteValue) {
					if(nextType == 1 && (nextValue == 0 || nextValue == 1 || nextValue == 2 || nextValue == 3 || nextValue == 4 || nextValue == 5)) {
						std::cout << "Syntax error(section: " << i+1 << ") (No preceeding number): " << error_call(current) << std::endl; //No preceeding number
						error = true;
					}
					if(current->next && current->next->type == 1 && current->next->value == 8) {
						std::cout << "Syntax error(section: " << i+1 << ") (Empty Parenthesis block): " << error_call(current) << std::endl;
						error = true;
					}
				} else {
					if(current->next && current->next->type == 0) {
						std::cout << "Syntax error(section: " << i+1 << ") (Unexpected number): " << error_call(current) << " (If you were attempting to multiply them together, just throw in a multiplication symbol (*))" << std::endl;
						error = true;	
					} if(current->next && current->next->type == 1 && (nextValue == 9 || nextValue == 10 || nextValue == 11)) {
						std::cout << "Syntax error(section: " << i+1 << ") (Unexpected trig function): " << error_call(current) << " (If you were attempting to multiply them together, just throw in a multiplication symbol (*))" << std::endl;
						error = true;
					}
				}
				inAbsoluteValue = !inAbsoluteValue;
			} else if(value == 0 || value == 1 || value == 2 || value == 3 || value == 4 || value == 5) {
				if(i == 0) { 
					std::cout << "Syntax error(section: " << i+1 << ") (No preceeding number): " << error_call(current) << std::endl; //No preceeding number
					error = true;
				} if(current->next == NULL || (current->next != NULL && ((nextType == 1 && nextValue == 7) || (nextType == 1 && nextValue == 8 && inAbsoluteValue)))) {
					std::cout << "Syntax error(section: " << i+1 << ") (No following number): " << error_call(current) << std::endl; //No following number
					error = true;
				} if(current->next != NULL && (nextType == 1 && (nextValue == 0 || nextValue == 1 || nextValue == 2 || nextValue == 3 || nextValue == 4 || nextValue == 5))) { //Double symbols
					std::cout << "Syntax error(section: " << i+1 << ") (double symbols): " << error_call(current) << std::endl; //double symbols
					error = true;
				}
			}
		} else if(type == 0) { // A number
			//if(current->next) { std::cout << current->next->type << std::endl; }

			if(current->next != NULL && (current->next->type == 1 && (nextValue == 9 || nextValue == 10 || nextValue == 11))) {
				std::cout << "Syntax error(section: " << i+1 << ") (unexpected trig function): " << error_call(current) << " (If you were attempting to multiply them together, just throw in a multiplication symbol (*))" << std::endl;
				error = true;
	
			}
			if(current->next != NULL && (current->next->type == 1 && (nextValue == 6 || (nextValue == 8 && !inAbsoluteValue)))) {
				std::cout << "Syntax error(section: " << i+1 << ") (unexpected parenthesis or absolute value): " << error_call(current) << std::endl;
				error = true;
			}
		}

		i++;
		current = current->next;
		
	}

	if(parenthesisDepth != 0) {
		std::cout << "Syntax error (Unclosed parenthesis)" << std::endl;
		error = true;
	} if(inAbsoluteValue) {
		std::cout << "Syntax error (Unclosed absolute value)" << std::endl;
		error = true;
	}

	if(error) { exit(-2); } //Exit if there was one

	return list->length;

}
