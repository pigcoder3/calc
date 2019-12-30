//Standard
#include <cmath>

//libs
#include "gtest/gtest.h"

//Project
#include "../src/calc.h"

//Needed functions

bool disableSyntaxChecks = true; //We wont need this here because I know the syntax is correct

//int testSize = 20;

long double truncate_double(long double val) {
	std::string asString = std::to_string(val);
	asString.resize(7);
	return stod(asString);
}

void parseCheck(long double values[], int size) {

	struct node *current = list->root;

	EXPECT_EQ(list->length, size); //The things should be the same size

	int i = 0;
	while(current != NULL && i <= size+1) {
		EXPECT_EQ(values[i], current->value);
		current = current->next;
		i++;
	}


}

/* Test Suite: Parse
 *
 *	Test to ensure that the parser is
 *	correctly parsing each character to the correct symbol.
*/

TEST(Parse, addition) {

	//Setup
	char *expression = (char*)("4+5");
	long double listValues[] = {4, 0, 5};

	parse(expression); //The expression is stored in the linked list

	//Testing
	parseCheck(listValues, sizeof(listValues)/sizeof(long double));

	//Teardown
	list->clean();
}

TEST(Parse, subtraction) {

	//Setup
	char *expression = (char*)("4-5");
	long double listValues[] = {4, 1, 5};
	
	parse(expression); //The expression is stored in the linked list

	//Testing
	parseCheck(listValues, sizeof(listValues)/sizeof(long double));

	//Teardown
	list->clean();
}

TEST(Parse, multiplication) {

	//Setup
	char *expression = (char*)("4*5");
	long double listValues[] = {4, 2, 5};

	parse(expression); //The expression is stored in the linked list

	//Testing
	parseCheck(listValues, sizeof(listValues)/sizeof(long double));

	//Teardown
	list->clean();
}

TEST(Parse, division) {

	//Setup
	char *expression = (char*)("4/5");
	long double listValues[] = {4, 3, 5};
	
	parse(expression); //The expression is stored in the linked list

	//Testing
	parseCheck(listValues, sizeof(listValues)/sizeof(long double));

	//Teardown
	list->clean();
}

TEST(Parse, doubleNegative) {

	//Setup
	char *expression = (char*)("4--5");
	long double listValues[] = {4, 1, -5};

	parse(expression); //The expression is stored in the linked list

	//Testing
	parseCheck(listValues, sizeof(listValues)/sizeof(long double));

	//Teardown
	list->clean();
}

TEST(Parse, bareDecimal) {

	//Setup
	char *expression = (char*)("4*.5");
	long double listValues[] = {4, 2, 0.5};

	parse(expression); //The expression is stored in the linked list

	//Testing
	parseCheck(listValues, sizeof(listValues)/sizeof(long double));

	//Teardown
	list->clean();
}

TEST(Parse, negativeBareDecimal) {

	//Setup
	char *expression = (char*)("4*-.5");
	long double listValues[] = {4, 2, -0.5};

	parse(expression); //The expression is stored in the linked list

	//Testing
	parseCheck(listValues, sizeof(listValues)/sizeof(long double));

	//Teardown
	list->clean();
}

TEST(Parse, negativeDecimal) {

	//Setup
	char *expression = (char*)("4+-.5");
	long double listValues[] = {4, 0, -0.5};

	parse(expression); //The expression is stored in the linked list

	//Testing
	parseCheck(listValues, sizeof(listValues)/sizeof(long double));

	//Teardown
	list->clean();
}

TEST(Parse, nonBareDecimal) {

	//Setup
	char *expression = (char*)("4+0.5");
	long double listValues[] = {4, 0, 0.5};

	parse(expression); //The expression is stored in the linked list

	//Testing
	parseCheck(listValues, sizeof(listValues)/sizeof(long double));

	//Teardown
	list->clean();
}

TEST(Parse, trigFunctions) {

	//Setup
	char *expression = (char*)("sin(1)+cos(1)+tan(1)");
	long double listValues[] = {9, 6, 1, 7, 0, 10, 6, 1, 7, 0, 11, 6, 1, 7};

	parse(expression); //The expression is stored in the linked list

	//Testing
	parseCheck(listValues, sizeof(listValues)/sizeof(long double));

	//Teardown
	list->clean();
}

/* Test Suite: Calculate
 *
 *	Test to ensure each of the following
 * 	operations are working correctly:
 *	 - Addition
 *	 - Subtraction
 *	 - Multiplication
 *	 - Division
 *   - Exponents
 *   - Roots
 *   - Absolute value
 *   - Parenthesis
*/

TEST(Calculate, addition) {

	//Setup
	char *expression = (char*)("4+5+6");
	int expressionSize = 6; //Note that this needs to the be size when it is in the linked list
	long double expected = 15;
	
	parse(expression); //The expression is stored in the linked list

	//Testing
	calculate(NULL, 0, expressionSize, false);
	ASSERT_EQ(expected, truncate_double(list->root->value));

	//Teardown
	list->clean();
	
}

TEST(Calculate, subtraction) {

	//Setup
	char *expression = (char*)("4-5-6");
	int expressionSize = 6; //Note that this needs to the be size when it is in the linked list
	long double expected = -7;
	
	parse(expression); //The expression is stored in the linked list

	//Testing
	calculate(NULL, 0, expressionSize, false);
	ASSERT_EQ(expected, truncate_double(list->root->value));

	//Teardown
	list->clean();
	
}

TEST(Calculate, subtractNegative) {

	//Setup
	char *expression = (char*)("4--5");
	int expressionSize = 3; //Note that this needs to the be size when it is in the linked list
	long double expected = 9;
	
	parse(expression); //The expression is stored in the linked list

	//Testing

	calculate(NULL, 0, expressionSize, false);
	ASSERT_EQ(expected, truncate_double(list->root->value));

	//Teardown
	list->clean();
	
}

TEST(Calculate, multiplication) {

	//Setup
	char *expression = (char*)("4*5*6");
	int expressionSize = 6; //Note that this needs to the be size when it is in the linked list
	long double expected = 120;

	parse(expression); //The expression is stored in the linked list

	//Testing
	calculate(NULL, 0, expressionSize, false);
	ASSERT_EQ(expected, truncate_double(list->root->value));

	//Teardown
	list->clean();

}

TEST(Calculate, division) {

	//Setup
	char *expression = (char*)("10/5");
	int expressionSize = 6; //Note that this needs to the be size when it is in the linked list
	long double expected = 2; //note the rounding

	parse(expression); //The expression is stored in the linked list

	//Testing
	calculate(NULL, 0, expressionSize, false);
	ASSERT_EQ(expected, truncate_double(list->root->value));

	//Teardown
	list->clean();

}

TEST(Calculate, exponent) {

	//Setup
	char *expression = (char*)("4^3");
	int expressionSize = 3; //Note that this needs to the be size when it is in the linked list
	long double expected = 64; //note the rounding

	parse(expression); //The expression is stored in the linked list

	//Testing
	calculate(NULL, 0, expressionSize, false);
	ASSERT_EQ(expected, truncate_double(list->root->value));

	//Teardown
	list->clean();

}

TEST(Calculate, root) {

	//Setup
	char *expression = (char*)("4*(3V8)");
	int expressionSize = 7; //Note that this needs to the be size when it is in the linked list
	long double expected = 8; //note the rounding

	parse(expression); //The expression is stored in the linked list

	//Testing
	calculate(NULL, 0, expressionSize, false);
	ASSERT_EQ(expected, truncate_double(list->root->value));

	//Teardown
	list->clean();

}

TEST(Calculate, parenthesis) {

	//Setup
	char *expression = (char*)("4*(4+4)");
	int expressionSize = 8; //Note that this needs to the be size when it is in the linked list
	long double expected = 32;

	parse(expression); //The expression is stored in the linked list

	//Testing
	calculate(NULL, 0, expressionSize, false);
	ASSERT_EQ(expected, truncate_double(list->root->value));

	//Teardown
	list->clean();

}

TEST(Calculate, nestedParenthesis) {

	//Setup
	char *expression = (char*)("4*(4+4*(4+4))");
	int expressionSize = 13; //Note that this needs to the be size when it is in the linked list
	long double expected = 144;

	parse(expression); //The expression is stored in the linked list

	//Testing
	calculate(NULL, 0, expressionSize, false);
	ASSERT_EQ(expected, truncate_double(list->root->value));

	//Teardown
	list->clean();

}

TEST(Calculate, trigFunctions) {

	//Setup
	char *expression = (char*)("sin(1)+cos(1)+tan(1)");
	int expressionSize = 14; //Note that this needs to the be size when it is in the linked list
	long double expected = 2.93918;

	parse(expression); //The expression is stored in the linked list

	//Testing
	calculate(NULL, 0, expressionSize, false);
	ASSERT_EQ(expected, truncate_double(list->root->value));

	//Teardown
	list->clean();

}
