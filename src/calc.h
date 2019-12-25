#ifndef CALC_H
#define CALC_H

extern bool showSteps;
extern bool sciNotation;
extern bool debug;
extern int depth;
extern int listLength;

struct node {
	int type;
	long double value;
	struct node *next;
};

extern struct node *root;
extern int lastNumberGottenLength;

//This is never implimented, so i have no idea why this is here.
//std::string recreateEquation(std::string equation, int index, int calculationStartIndex, long double newNumber);

//Returns a string number as a number
long double getNumberAsNumber(std::string input, int index);

//Returns a string number for printing
std::string getNumberAsString(std::string input, int index);

//Replaces a list of nodes within an array with another list of nodes
//TODO: optimize this thing cuz its mega slow
void replace_nodes(int beforeIndex, int startIndex, int afterIndex, long double result, int length);

//Removes a node at the given index
void removeNode(int index);

//Adds a new node in the index given
void insertNode(int index, struct node* newNode);

//Jumps to a specific index in the linked list
struct node* jumpTo(int i);

//Prints every element in the list to std::cout
std::string printLinkedList();

//Returns the length of the linked list
//Not used anymore in favor of a less costly way other than in testing
int linkedListLength();

//Removes any preceding or following zeroes that are not needed
//Used only when printing steps to make things look nicer
std::string removeZeros(std::string input);

//Returns a symbol based on the value stored in the node
char getSymbol(int value);

//This will print each step in the equation, showing users how it is calculated
//Only done if the -s option is used
void printStep(int begin, int length);

//frees all pointers within the linked list
void clean();

//Perform a calculation for a sub_calculation
void calculate(struct node *sub_root_last, int startIndex, int length, bool absolute_value);

long double getNumberAsNumber(std::string input, int index);

struct node* create_node(int type, int value);

std::string error_call(struct node *current);

int parse(char *equation);

#endif
