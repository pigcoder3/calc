#include <cstring>
#include <iostream>
#include <algorithm>
#include <cmath>

bool showSteps = false;
bool sciNotation = false;
int depth = 0;

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

void printStep(std::string step);
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

void calculate(struct node *sub_root_last, int startIndex, int length, bool absolute_value) {

	//std::cout << "In calculation" << std::endl;

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
	struct node *next_calculation_root = (struct node*)malloc(sizeof(struct node));
	struct node *calculation_start_node_last = sub_root_last;
	struct node *calculation_start_node = (struct node*)malloc(sizeof(struct node));
	struct node *calculation_end_node = (struct node*)malloc(sizeof(struct node));
	int calculationStartIndex = 0;
	bool inCalculation = 0;	
	long double result;
	while(true) {

		//if(showSteps && !scoutingPhase) { printStep(equation); }
		
		if(scoutingPhase) { //Reset everything to scout again
			parenthesis = false;
			absoluteValue = false;
			exponentsOrRoots = false;
			multiplyOrDivide = false;
			addOrSubtract = false;
			inParenthesis = false;
			inAbsoluteValue = false;
			calculation_start_node = 0;
			calculation_end_node = 0;
			calculationStartIndex = 0;
			inCalculation = 0;
		}

		struct node *current = jumpTo(startIndex);
		int i=0;
		
		//std::cout << "Cycling again, scouting phase: " << scoutingPhase << std::endl;

		while(current != NULL && i < length) {

			int type = current->type;
			long double value = current->value;

			if(!inCalculation && !inParenthesis && !inAbsoluteValue && type == 0) {
				calculationStartIndex = i;
				calculation_start_node = current;
				//std::cout << "Beginning to read a calculation (Number found)" << std::endl;
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

					calculate(calculation_start_node_last, calculationStartIndex+startIndex, i-calculationStartIndex, false); 
						
					inParenthesis = false;
					break;
				} else if(value == 9) { //When closing absolute value in calculation phase, use recursion to calculate what is inside.
					if(!scoutingPhase) {
						if(inAbsoluteValue) { //The end of the absolute value block
							//Rewrite this to get a sublist
							
							calculate(calculation_start_node_last, calculationStartIndex+startIndex, i-calculationStartIndex, true);
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
							
							//if(!scoutingPhase) { std::cout << "Exponent" << std::endl; }
							//std::cout << "Exponent: " << equation.substr(calculationStartIndex, i) << endl;
							//Rewrite this to get a sublist
							try {
								result = pow(last_node->value, current->next->value);
								replace_nodes(calculationStartIndex-1, calculationStartIndex, i+2, result, 3);
								length-=2;
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (When using exponents)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (When using exponents)" << std::endl;
								exit(-1);
							}
	
						} else if(value == 4) {
							
							//if(!scoutingPhase) { std::cout << "Root" << std::endl; }
							//std::cout << "Root: " << equation.substr(calculationStartIndex, i) << endl;
							//Rewrite this to get a sublist
							try {
								result = pow(current->next->value, 1.0/(last_node->value));
								replace_nodes(calculationStartIndex-1, calculationStartIndex, i+2, result, 3);
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
							
							//if(!scoutingPhase) { std::cout << "Multiplication" << std::endl; }
							//Rewrite this to get a sublist
							try {
								result = last_node->value * current->next->value;
								replace_nodes(calculationStartIndex-1, calculationStartIndex, i+2, result, 3);
								length-=2;
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During multiplication)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During multiplication)" << std::endl;
								exit(-1);
							}
						} else { //Divide
							//if(!scoutingPhase) { std::cout << "Division" << std::endl; }
							//Rewrite this to get a sublist
							try {
								result = last_node->value / current->next->value;
								replace_nodes(calculationStartIndex-1, calculationStartIndex, i+2, result, 3);
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
					//std::cout << "parenthesis: " << parenthesis << " inParenthesis: " << inParenthesis << " absoluteValue: " << absoluteValue << " inAbsoluteValue: " << inAbsoluteValue << " exponentsOrRoots: " << exponentsOrRoots << " multiplyOrDivide: " << multiplyOrDivide << std::endl;
					if(((parenthesis && !inParenthesis) || (absoluteValue && !inAbsoluteValue) || exponentsOrRoots || multiplyOrDivide)) {
						//std::cout << "Cannot add! Must follow order of operations" << std::endl;
						i++; //for loop index increase
						last_node = current;
						current = current->next;
						inCalculation = false; 
						continue;
					} //Dont use this as part of the equation if there is something higher in PEMDAS
					if(!scoutingPhase && !parenthesis && !exponentsOrRoots && !multiplyOrDivide) { //Make sure the other parts of PEMDAS that come first do not exist
						if(value == 0) { //Add
							//if(!scoutingPhase) { std::cout << "Addition" << std::endl; }
							//Rewrite this to get a sublist
							try {
								result = last_node->value + current->next->value;
							//	std::cout << calculationStartIndex << std::endl;
								replace_nodes(calculationStartIndex-1, calculationStartIndex, i+2, result, 3);
								length-=2;	
							} catch(std::invalid_argument) {
								std::cout << "Error: invalid syntax. (During addition)" << std::endl;
								exit(-1);
							} catch(std::out_of_range) {
								std::cout << "Error: Number too large. (During addition)" << std::endl;
								exit(-1);
							}
						} else { //Subtract
							//if(!scoutingPhase) { std::cout << "Subtraction" << std::endl; }
							//Rewrite this to get a sublist
							try {
								result = last_node->value - current->next->value;
								replace_nodes(calculationStartIndex-1, calculationStartIndex, i+2, result, 3);
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
				if(sub_root_last != NULL) {
					sub_root_last->next->value = abs(result);
				} else {
					root->value = abs(result);
				}
			}

			return;

		} //Note the "!scoutingPhase", we just left it
	}
}

void replace_nodes(int beforeIndex, int startIndex, int afterIndex, long double result, int length) {
	//std::cout << "replacing nodes" << std::endl;
	//Place the result in the linked list
	struct node *result_node = (struct node*)malloc(sizeof(struct node));
	//std::cout << result << std::endl;
	result_node->type = 0;
	result_node->value = result;

	for(int i=0; i<length; i++) {
		removeNode(startIndex);
	}

	insertNode(startIndex, result_node);

	/* std::cout << "Before insertion" << std::endl;
	current_temp = root;
	while(current_temp != NULL) {
		std::cout << "value: " << current_temp->value << " ; ";
		current_temp = current_temp->next;
	}

	//Put this in the equation
	std::cout << "bruh" << std::endl;	
	if(jumpTo(beforeIndex) != NULL) { //Determine if it is defined
		std::cout << "bruh" << std::endl;
		std::cout << "Before value: " << jumpTo(startIndex)->value << std::endl;
		std::cout << "Before type: " << jumpTo(startIndex)->type << std::endl;
		struct node *start = jumpTo(startIndex);
		start = result_node;
		result_node->next = jumpTo(afterIndex);
	} else { //If this is the beginning of the equation, then set the root
		std::cout << "bruh2" << std::endl;
		result_node->next = jumpTo(afterIndex);
		root = result_node;	
	}

	//free(current_temp);
	//free(before);*/

	printLinkedList();

}

void removeNode(int index) {

	//std::cout << "removing node: " << index << std::endl;

	struct node *current = root;
	struct node *last = (struct node*)malloc(sizeof(struct node));

	int i=0;

	while(current->next != NULL && i < index) { //Get to the right place
		i++;
		last = current;
		current = current->next;	
	}


	if(i == 0 && current->next != NULL) {
		//std::cout << "The removed node is at the front" << std::endl;
		root = current->next;
		root->next = current->next->next;
	} else {
		//std::cout << "The removed node is somewhere inside the list" << std::endl;
		last->next = current->next; //Remove it
	}

	current = NULL;

	printLinkedList();

}

void insertNode(int index, struct node* newNode) {
	
	//std::cout << "inserting node: " << index << std::endl;	
	//std::cout << "new node value: " << newNode->value << std::endl;

	struct node *current = root;

	int i=0;

	while(current != NULL && i < index-1) { //Get to the right place
		i++;
		current = current->next;	
	}

	//std::cout << "length: " << linkedListLength() << " i: " << i <<  std::endl;

	if(i == 0) {
		//std::cout << "The added node will be at the front" << std::endl;
		newNode->next = root; 
		root = newNode;
	} else {
		//std::cout << "The added node will be somewhere inside the list" << std::endl;
		if(i < linkedListLength()) {
			newNode->next = current->next; //Remove it
			current->next = newNode;
		} else {
			newNode->next = NULL;
		}
		//current->next = (struct node*)malloc(sizeof(struct node));
		//current->next = newNode;
	}

	printLinkedList();

}

int linkedListLength() {
	
	struct node *current = root;

	int len = 0;

	while(current != NULL) {
		//std::cout << current->value << std::endl;
		current = current->next;
		len++;
	}

	return len;

}

void printLinkedList() {

	struct node *current = root;

	//std::cout << "Printing linked list: ";

	while(current != NULL) {
		//std::cout << current->value << ", ";
		current = current->next;
	}

	//std::cout << std::endl;

}

struct node* jumpTo(int i) {
	
	struct node *current = root;

	//std::cout << "Jumping to index: " << i << std::endl;

	int index=0;
	while(current != NULL && index < i) {
		current = current->next;
	}

	return current;

}

void printStep(std::string step) {

	for(int i=0; i<depth; i++) {
		//std::cout << "  "; //Print out a spacing
	}
	//std::cout << step << std::endl; //Print out the step
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
		//std::cout << "Error: Invalid syntax. (Unkown origin)" << std::endl;
		exit(-1);
	} catch(std::out_of_range) {
		//std::cout << "Error: Number too large. (Unknown origin)" << std::endl;
		exit(-1);
	}

}

std::string getNumberAsString(std::string input, int index) {
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

	//std::cout << number.length() << std::endl;
	//std::cout << number << std::endl;

	return number;

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

	//Remove leading zeros
	for(int i=0; i<input.length(); i++) {
		if(input[i] == '.') {
			break;
		} else if(input[i] == '-') {
			//We dont want to remove the negative sign so do nothing
		} else if(input[i] == '0') {
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
	if(current->next != NULL && current->type == 1) {
		output+=getSymbol(current->next->value);
	} else {
		output+=removeZeros(std::to_string(current->next->value));
	}

	return output;
}

struct node* create_node(int type, int value) {

	struct node *node = (struct node*)malloc(sizeof(struct node));
	node->type = type; //Set its values
	node->value = value;
	return node;

}

int main(int argc, char **argv) {

	//If the incorrect number of arguments were given, give the usage
	if(argc < 2 || argc > 4) { 
		std::cout << "Usage: calc equation [-s] [-n]\n";
		return 0;
	}

	for(int i = 0; i < argc; i++) {
	
		//Send the help message
		if(strncmp(argv[i], "--help", strlen("--help")) == 0 || strncmp(argv[i], "-h", strlen("-h")) == 0) {
			const char* help = "[HELP]\n"
				"Usage: calc equation [-s] [-n]\n"
				"       calc -h\n"
				"\n"
				"Options:\n"
				"  -h - show this help message.\n"
				"  -s - show steps to solve.\n"
				"  -n - show result in scientific notation.\n"
				"\n"
				"Notes:\n"
				"  [1] Quotation marks should be present to make sure that your shells interprets the equation as a single argument.\n"
				"  [2] Calc follows rules of order of operations.\n"
				"  [3] If certain symbols are not placed in their proper positions (or not placed at all), the parser will have incorrect responses (without an error because none will be detected).\n"
				"    Ex: 4*5(3+2) will output 200. This is because the program will place the output of the stuff within the parenthesis back in the equation (So it will read 4*55).\n"
				"      To make this work, do 4*5*(3+2) to get the intended result.\n"
				"    Ex: 4+/7 will output 0.571429. This is because the parser will completely ignore the + sign. (This is completely improper syntax anyway).\n"
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
		}
	}
	//Remove all spaces
	std::string str = argv[1];
	str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

	struct node *current = root;

	//Turn the equation into a linked list
	for(int i=0; i<str.length(); i++) {
		
		char c = str[i];
		//std::cout << c << std::endl;
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
		} else if(c == '-') { //This one will need some extra work
			if(!isdigit(str[i+1]) && !(str[i+1] == '.' && isdigit(str[i+2]))) { //Next thing is not a number, so this is a minus sign
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

	}

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
		if(current->next != NULL) {
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
			} else if(value == 7) {
				if(parenthesisDepth == 0) {
					std::cout << "Syntax error(section: " << i+1 << ") (Extra parenthesis): " << error_call(current) << std::endl;
					error = true;
				} else {
					parenthesisDepth--;
				}
			} else if(value == 8) {
				if(!inAbsoluteValue) {
					if(nextValue == 0 || nextValue == 1 || nextValue == 2 || nextValue == 3 || nextValue == 4 || nextValue == 5) {
						std::cout << "Syntax error(section: " << i+1 << ") (No preceeding number): " << error_call(current) << std::endl; //No preceeding number
						error = true;
					}
				}
			} else if(value == 0 || value == 1 || value == 2 || value == 3 || value == 4 || value == 5) {
				if(i == 0) { 
					std::cout << "Syntax error(section: " << i+1 << ") (No preceeding number): " << error_call(current) << std::endl; //No preceeding number
					error = true;
				} if(current->next == NULL || (current->next != NULL && ((nextType == 1 && nextValue == 7) || (nextValue == 8 && inAbsoluteValue)))) {
					std::cout << "Syntax error(section: " << i+1 << ") (No following number): " << error_call(current) << std::endl; //No following number
					error = true;
				} if(current->next != NULL && (nextType == 1 && (nextValue == 0 || nextValue == 1 || nextValue == 2 || nextValue == 3 || nextValue == 4 || nextValue == 5))) { //Double symbols
					std::cout << "Syntax error(section: " << i+1 << ") (double symbols): " << error_call(current) << std::endl; //double symbols
					error = true;
				}
			}
		} else if(type == 0) {
			if(current->next != NULL && (nextValue == 6 || (nextValue == 8 && !inAbsoluteValue))) {
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

	/*
	//Check for syntax errors
	bool inAbsoluteValue = false;
	int parenthesisDepth = 0;
	bool error = false;
	for(int i=0; i<str.length(); i++) {
		char c = str[i];
		if(c == '(') {
			char c2 = str[i+1];
			if(c2 == '+' || (c2 == '-' && (!isdigit(str[i+2]) && !(str[i+2] == '.' && isdigit(str[i+3])))) || c2 == '*' || c2 == '/' || c2 == '^' || c2 == 'V') { //extra symbol
				std::cout << "Syntax error(char: " << i+1 << ") (No preceeding number): " << c << c2 << std::endl; //No preceeding number
				error = true;
			}
			parenthesisDepth++;
		} else if(c == ')') {
			if(parenthesisDepth == 0) {
				std::cout << "Syntax error(char: " << i+1 << ") (Extra parenthesis): " << c << std::endl;
				error = true;
			} else {
				parenthesisDepth--;
			}
		} else if(c == '|') {	
			char c2 = str[i+1];
			if(!inAbsoluteValue) {
				if(c2 == '+' || (c == '-' && !isdigit(str[i+1])) || c2 == '*' || c2 == '/' || c2 == '^' || c2 == 'V') { //extra symbol
					std::cout << "Syntax error(char: " << i+1 << ") (No preceeding number): " << c << c2 << std::endl; //No preceeding number
					error = true;
				}
			}
			inAbsoluteValue = !inAbsoluteValue;
		} else if(c == '+' || (c == '-' && (!isdigit(str[i+1]) && !(str[i+1] == '.' && isdigit(str[i+2])))) || c == '*' || c == '/' || c == '^' || c == 'V') {
			if(i == 0) { 
				std::cout << "Syntax error(char: " << i+1 << ") (No preceeding number): " << c << std::endl; //No preceeding number
				error = true;
			} if(i+1 == str.length()) {
				std::cout << "Syntax error(char: " << i+1 << ") (No following number): " << c << std::endl; //No following number
				error = true;
			}
			char c2 = str[i+1];
			if(c2 == ')' || (c2 == '|' && inAbsoluteValue)) { //Extra symbol before section end
				std::cout << "Syntax error(char: " << i+1 << ") (No following number): " << c << c2 << std::endl; //Not following number
				error = true;
			} else if(c2 == '+' || (c2 == '-' && (!isdigit(str[i+2]) && !(str[i+2] == '.' && isdigit(str[i+3])))) || c2 == '*' || c2 == '/' || c2 == '^' || c2 == 'V') { //Double symbols
				std::cout << "Syntax error(char: " << i+1 << ") (double symbols): " << c << c2 << std::endl; //double symbols
				error = true;
			}
		} else if(c == '.') { //Extra decimal
			char c1 = str[i-1];
			char c2 = str[i+1];
			if((c1 == '.' || c2 == '.') || (!isdigit(c1) && !isdigit(c2))) {
				std::cout << "Sytnax error(char: " << i+1 << ") (useless decimal point): " << c1 << c << c2 << std::endl; //Make sure there are no useless decimal points anywhere
				error = true;
			}
		} else if(isdigit(c)) {
			char c2 = str[i+1];
			if(c2 == '(' || (c2 == '|' && !inAbsoluteValue)) {
				std::cout << "Syntax error(char: " << i+1 << ") (unexpected parenthesis or absolute value): " << c << c2 << std::endl;
				error = true;
			}
		} else if(!isdigit(c) && c != '.' && !(c == '-' && isdigit(str[i+1]))) {
			std::cout << "Syntax error(char: " << i+1 << ") (unknown symbol): " << c << std::endl; //Unknown symbol
			error = true;
		}
	}

	if(parenthesisDepth != 0) {
		std::cout << "Syntax error (Unclosed parenthesis)" << std::endl;
		error = true;
	} if(inAbsoluteValue) {
		std::cout << "Syntax error (Unclosed absolute value)" << std::endl;
		error = true;
	}

	if(error) { exit(-2); } //Exit if there was one
	*/

	//Begin recursive calculations
	
	if(sciNotation) { //Print the result in scientific notation
		std::cout << std::scientific;
	}
	calculate(NULL, 0, i, false);
	std::cout << root->value << std::endl;

	return 0;
}
