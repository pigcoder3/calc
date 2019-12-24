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

void calculate(struct node *sub_root_last, int startIndex, int length, bool absolute_value);

long double getNumberAsNumber(std::string input, int index);

struct node* create_node(int type, int value);

std::string error_call(struct node *current);

int parse(char *equation);

#endif
