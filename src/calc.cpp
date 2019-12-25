#include <cstring>
#include <iostream>
#include <algorithm>
#include <cmath>

bool showSteps = false;
bool sciNotation = false;
bool debug = false;
bool disableSyntaxCheck = false;
int depth = 0;
int listLength = 0; //NOTE: This is not the same this as length, which is the length of the current calculation

struct node {
	int type;
	long double value;
	struct node *next;
};

struct node *root = (struct node*)malloc(sizeof(struct node));

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

std::string recreateEquation(std::string equation, int index, int calculationStartIndex, long double newNumber);
long double getNumberAsNumber(std::string input, int index);
std::string getNumberAsString(std::string input, int index);
std::string removeZeros(std::string input);
void replace_nodes(int beforeIndex, int startIndex, int afterIndex, long double result, int length);
void removeNode(int index);
void insertNode(int index, struct node* newNode);
struct node* jumpTo(int i);
void printLinkedList();
int linkedListLength();
std::string removeZeros(std::string input);
char getSymbol(int value);
void printStep(int begin, int length);

void calculate(struct node *sub_root_last, int startIndex, int length, bool absolute_value) {

	if(debug) {
		std::cout << "In calculation" << std::endl;
		std::cout << "depth: " << depth << std::endl;
	}

	if(depth > 0) { //Remove any paarenthesis or absolute value symbols to prevent infinite recursion
		//std::cout << "Removing parenthesis and absolute value symbols" << std::endl;
		removeNode(startIndex);
		length--;
		removeNode(startIndex + length);
		length--;
	}

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
	struct node *last_node = sub_root_last;	
	struct node *calculation_start_node_last = sub_root_last;
	int calculationStartIndex = 0;
	bool inCalculation = 0;	
	long double result;
	while(true) {

		if(showSteps && !scoutingPhase) { printStep(startIndex, length); }
		
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

		struct node *current = jumpTo(startIndex);
		int i=0;

		while(current != NULL && i <= length) {

			int type = current->type;
			long double value = current->value;

			if(!inCalculation && !inParenthesis && !inAbsoluteValue && type == 0) {
				calculationStartIndex = i;
				if(debug) { std::cout << "Beginning to read a calculation (Number found)" << std::endl; }
				calculation_start_node_last = last_node;
				inCalculation = true;
			}

			if(type == 1) {
				if(value == 6) { //Getting the open parenthesis
					calculationStartIndex = i;
					inParenthesis = true;
					parenthesis = true;
					inCalculation = true;
				} else if(!scoutingPhase && value == 7 && inParenthesis) { //When closing parenthesis in calculation phase, use recursion to calculate what is inside
					depth++;

					calculate(calculation_start_node_last, startIndex+calculationStartIndex, i-calculationStartIndex, false); 
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
					} //Dont use this as part of the equation if there is something higher in PEMDAS
					if(!scoutingPhase && !parenthesis) { //Make sure the other parts of PEMDAS that come first do not exist
						if(value == 5) {
							
							if(debug && !scoutingPhase) { std::cout << "Exponent" << std::endl; }
							try {
								result = pow(last_node->value, current->next->value);
								replace_nodes(calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
								length-=2;
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (When using exponents)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (When using exponents)" << std::endl;
								exit(-1);
							}
	
						} else if(value == 4) {
							
							if(debug && !scoutingPhase) { std::cout << "Root" << std::endl; }
							try {
								result = pow(current->next->value, 1.0/(last_node->value));
								replace_nodes(calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
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
				} else if(value == 2 || value == 3) { //Make sure the other parts of PEMDAS that come first do not exist
					if(scoutingPhase) {
						multiplyOrDivide = true;
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
					} //Dont use this as part of the equation if there is something higher in PEMDAS
					else if(!scoutingPhase && !parenthesis && !exponentsOrRoots) {
						if(value == 2) { //Multiply
							
							if(debug && !scoutingPhase) { std::cout << "Multiplication" << std::endl; }
							try {
								result = last_node->value * current->next->value;
								replace_nodes(calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
								length-=2;
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During multiplication)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During multiplication)" << std::endl;
								exit(-1);
							}
						} else { //Divide
							if(debug && !scoutingPhase) { std::cout << "Division" << std::endl; }
							try {
								result = last_node->value / current->next->value;
								replace_nodes(calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
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
					if(((parenthesis && !inParenthesis) || (absoluteValue && !inAbsoluteValue) || exponentsOrRoots || multiplyOrDivide)) {
						i++; //for loop index increase
						last_node = current;
						current = current->next;
						inCalculation = false; 
						continue;
					} //Dont use this as part of the equation if there is something higher in PEMDAS
					if(!scoutingPhase && !parenthesis && !exponentsOrRoots && !multiplyOrDivide) { //Make sure the other parts of PEMDAS that come first do not exist
						if(value == 0) { //Add
							if(debug && !scoutingPhase) { std::cout << "Addition" << std::endl; }
							try {
								result = last_node->value + current->next->value;
								replace_nodes(calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
								length-=2;	
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During addition)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During addition)" << std::endl;
								exit(-1);
							}
						} else { //Subtract
							if(debug && !scoutingPhase) { std::cout << "Subtraction" << std::endl; }
							try {
								result = last_node->value - current->next->value;
								replace_nodes(calculationStartIndex-1, calculationStartIndex+startIndex, startIndex+i+2, result, 3);
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
		if(!scoutingPhase && !parenthesis && !absoluteValue && !exponentsOrRoots && !multiplyOrDivide && !addOrSubtract) { 
			depth--; 

			//There is no need to replace any more nodes because only 1 remains.

			//Do absolute value if this is an absolulte value block
			if(absolute_value) {
				jumpTo(startIndex)->value = abs(result);
			}

			if(debug) { std::cout << "Done with this calculation" << std::endl; }

			return;

		} //Note the "!scoutingPhase", we just left it
	}
}

void replace_nodes(int beforeIndex, int startIndex, int afterIndex, long double result, int length) {
	if(debug) { std::cout << "replacing nodes" << std::endl; }
	//Place the result in the linked list
	struct node *result_node = (struct node*)malloc(sizeof(struct node));
	result_node->type = 0;
	result_node->value = result;

	//Figure out how to optimise this
	for(int i=0; i<length; i++) {
		removeNode(startIndex);
	}

	insertNode(startIndex, result_node);

	if(debug) { printLinkedList(); }

}

void removeNode(int index) {

	if(debug) { std::cout << "removing node: " << index << std::endl; }

	struct node *current = root;
	struct node *last = (struct node*)malloc(sizeof(struct node));

	int i=0;

	while(current != NULL && i < index) { //Get to the right place
		i++;
		last = current;
		current = current->next;	
	}


	if(i == 0) {
		if(debug) { std::cout << "The removed node is at the front" << std::endl; }
		if(current->next != NULL) {
			root = current->next;
			root->next = current->next->next;
		} else {
			root = NULL;
		}
	} else {
		if(debug) { std::cout << "The removed node is somewhere inside the list" << std::endl; }
		last->next = current->next; //Remove it
	}

	current = NULL;

	if(debug) { printLinkedList(); }

}

void insertNode(int index, struct node* newNode) {
	
	if(debug) {
		std::cout << "inserting node: " << index << std::endl;	
		std::cout << "new node value: " << newNode->value << " Index: " << index << std::endl;
	}

	struct node *current = root;

	int i=0;

	while(current != NULL && i < index-1) { //Get to the right place
		i++;
		current = current->next;	
	}

	if(i == 0) {
		if(debug) { std::cout << "The added node will be at the front" << std::endl; }
		newNode->next = root; 
		root = newNode;
	} else {
		if(debug) { std::cout << "The added node will be somewhere inside the list" << std::endl; }
		if(i < listLength) {
			newNode->next = current->next; //Remove it
			current->next = newNode;
		} else {
			newNode->next = NULL;
		}
	}

	if(debug) { printLinkedList(); }
	listLength++;

}

//I dont think I need this anymore
int linkedListLength() {
	
	struct node *current = root;

	int len = 0;

	while(current != NULL) {
		current = current->next;
		len++;
	}

	return len;

}

void printLinkedList() {

	struct node *current = root;

	while(current != NULL) {
		std::cout << current->value << ", ";
		current = current->next;
	}

	std::cout << std::endl;

}

struct node* jumpTo(int i) {
	
	struct node *current = root;

	if(debug) { std::cout << "Jumping to index: " << i << std::endl; }

	int index=0;
	while(current != NULL && index < i) {
		current = current->next;
		index++;
	}

	return current;

}

void printStep(int begin, int length) {

	for(int i=0; i<depth; i++) {
		std::cout << "  "; //Print out spacing
	}

	std::string output;

	//build the equation from the linked list
	struct node *current = jumpTo(begin);
	int i = 0;
	while(current != NULL && i <= length) {
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

char getSymbol(int value) {
	switch(value) {
		case 0:
			return '+';
		case 1:
			return '-';
		case 2:
			return '*';
		case 3:
			return '/';
		case 4:
			return 'V';
		case 5:
			return '^';
		case 6:
			return '(';
		case 7:
			return ')';
		case 8:
			return '|';
		default:
			return '?';
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

struct node* create_node(int type, int value) {

	struct node *node = (struct node*)malloc(sizeof(struct node));
	node->type = type; //Set its values
	node->value = value;
	node->next = NULL;
	return node;

}

//Free the entire list
void clean() {

	struct node *current = root;

	while(current) {
		node* last = current;	
		current = current->next;
	}

	root = NULL;

	listLength = 0;

}

//Parse the entire equation into a linked list
int parse(char *equation) {
	
	//Remove all spaces
	std::string str = equation;
	str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

	clean(); //Reset the thing (Just for convenience for tests)
	struct node *current = root;

	//Turn the equation into a linked list
	for(int i=0; i<str.length(); i++) {
		char c = str[i];
		if(c == '(') {
			struct node *node = create_node(1,6);
			if(i > 0){	
				current->next = node;
			} else {
				root = node;
			}
			current = node;
		} else if(c == ')') {
			struct node *node = create_node(1,7);
			if(i > 0){	
				current->next = node;
			} else {
				root = node;
			}
			current = node;
		} else if(c == '|') {
			struct node *node = create_node(1,8);
			if(i > 0){	
				current->next = node;
			} else {
				root = node;
			}
			current = node;
		} else if(c == 'V') {
			struct node *node = create_node(1,4);
			if(i > 0){	
				current->next = node;
			} else {
				root = node;
			}
			current = node;
		} else if(c == '^') {
			struct node *node = create_node(1,5);
			if(i > 0){	
				current->next = node;
			} else {
				root = node;
			}
			current = node;
		} else if(c == '/') {
			struct node *node = create_node(1,3);
			if(i > 0){	
				current->next = node;
			} else {
				root = node;
			}
			current = node;	
		} else if(c == '*') {
			struct node *node = create_node(1,2);
			if(i > 0){	
				current->next = node;
			} else {
				root = node;
			}
			current = node;
		} else if(c == '+') {
			struct node *node = create_node(1,0);
			if(i > 0){	
				current->next = node;
			} else {
				root = node;
			}
			current = node;	
		} else if(c == '-') {
			if (
			(i>0 && (isdigit(str[i-1]) || str[i-1] == ')' || str[i-1] == '|'))
			|| (!isdigit(str[i+1]) && !(str[i+1] == '.' && isdigit(str[i+2])) && !(str[i+1] == '-' && str[i+2] == '.' && isdigit(str[i+3])))
			|| (str[i+1] == '(' || str[i+1] == '|')
			) { //No number so this is is a negative
				struct node *node = create_node(1,1);
				if(i > 0){	
					current->next = node;
				} else {
					root = node;
				}
				current = node;	
			} else { //This is a number
				struct node *node = create_node(0, getNumberAsNumber(str, i));
				if(i > 0){	
					current->next = node;
				} else {
					root = node;
				}
				current = node;
				i+=lastNumberGottenLength-1;
			}
		} else if(isdigit(str[i]) || str[i] == '.') { //This is a number
			struct node *node = create_node(0, getNumberAsNumber(str, i));
			if(i > 0){	
				current->next = node;
			} else {
				root = node;
			}
			current = node;
			i+=lastNumberGottenLength-1;
		}

		listLength++;

	}

	if(debug) { std::cout << "New parsed equation: "; printLinkedList(); std::cout << std::endl; }

	if(!disableSyntaxCheck) { return listLength; } //All there is left to do here is syntax checking

	//Check for syntax errors
	bool inAbsoluteValue = false;
	int parenthesisDepth = 0;
	bool error = false;
	current = root;
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
		
		if(type == 1) {
			if(value == 6) { //Opening parenthesis
				if(current->next->type == 1 && current->next->value != 8) { //Only numbers can follow
					std::cout << "Syntax error(section: " << i+1 << ") (No preceeding number): " << error_call(current) << std::endl;
					error = true;
				}
				parenthesisDepth++;
			} else if(value == 7) { //Closing parenthesis
				if(parenthesisDepth == 0) {
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
					inAbsoluteValue = true;	
				} else {
					inAbsoluteValue = false;
				}
			/*
			} else if(value == 1) {
				/
				if(
					(i>0 && (isdigit(str[i-1]) || str[i-1] != '(' || str[i-1] == ')' || str[i-1] == '|'))
					|| (!isdigit(str[i+1]) && !(str[i+1] == '.' && isdigit(str[i+2])) && !(str[i+1] == '-' && str[i+2] == '.' && isdigit(str[i+3])))
					|| (str[i+1] == '(' || str[i+1] == '|')
				) { //Then it is a minus sign for sure
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
				} else { //It is a negative sign
						



*/
			} else if(value == 0 || value == 1 || value == 2 || value == 3 || value == 4 || value == 5) { //Note that the minus/negative is there
				if(i == 0 && value != 1) { 
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
		} else if(type == 0) {
			
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

	return listLength;

}
