/*********************************************************************
 *  Assignment 3: Parser                                             *
 *  COP 3402 Summer 2021                                             *
 *  Authors: Willow Maddox                                           *
 *           Michael Bernhardt                                       *
 *********************************************************************/

// Included libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

// Constants
#define TABLE_SIZE 1000
#define CONSTANT 1
#define VARIABLE 2
#define PROCEDURE 3
#define MAX_CHARS 12
#define BASE_ADDR 3

// Global variables and pointers
symbol *table;
lexeme *tokens;
lexeme currLex;
int sym_index;
int currToken;
int token_index;
int currLevel;
int currAddress;
int prevAddress;
int error;

// Functions
void program();
void block();
void statement();
void const_declaraton();
void var_declaration();
void proc_declaration();
void condition();
int rel_op();
void expression();
void term();
void factor();
int isTermOp();
int checkValidTokenAfterStatement();
int checkExpressionFollows();
void checkVarDeclared();
void checkConditionFollows();
int checkFactorFollows();
void errorCheck();
symbol initSymbol();
void upLevel();
void downLevel();
void enterSymbol(int, char*, int, int);
void getToken();
void printtable();
void errorend(int x);

// Parse a lexeme list into a symbol table
symbol *parse(lexeme *input)
{
	table = malloc(TABLE_SIZE * sizeof(symbol));
	sym_index = 0;
	error = 0;
	tokens = input;
	token_index = -1;
	currLevel = 0;
	currAddress = prevAddress = BASE_ADDR;

	// Start parsing with program call
	program();

	if (error)
	{
		free(table);
		return NULL;
	}
	else
	{
		printtable();
		return table;
	}
}

// Update the global varables to get the next token
void getToken()
{
	currToken = tokens[++token_index].type;
	currLex = tokens[token_index];
}

// Initialize and return a symbol
symbol initSymbol()
{
	symbol s;
	s.val = 0;
	s.level = 0;
	s.mark = 0;
	s.addr = 0;
	s.kind = 0;
	s.name[0] = '\0';

	return s;
}

// Enter a symbol into the symbol table
void enterSymbol(int type, char* name, int level, int val)
{
	// check for conflicting identifiers on same level
	for (int i = 0; i < sym_index; i++)
	{
		if (strcmp(table[i].name, name) == 0 && table[i].level == level)
		{
			errorend(1);
			exit(0);
		}
	}

	// Assign values to symbol
	symbol s;
	s.mark = 0;
	strcpy(s.name, name);
	s.kind = type;
	s.level = level;

	// Assign const type
	if (type == CONSTANT)
	{
		s.val = val;
	}
	else
	{
		s.val = 0;
	}
	
	// Assign var type
	if (type == VARIABLE)
	{
		s.addr = currAddress++;
	}
	else
	{
		s.addr = 0;
	}

	// Add sym to table
	table[sym_index] = s;
	sym_index++;
}

// Syntactic class - Top level of top down recursion parsing
void program()
{
	// Enter a symbol representing the main function
	enterSymbol(PROCEDURE, "main", 0, 0);
	getToken();

	block(); // Call for block

	// Make sure period at end of program
	if (currToken != periodsym)
	{
		errorend(3);
		exit(0);
	}
}

// Syntactic class for a block
void block()
{
	if (currToken == constsym) const_declaraton();
	
	if (currToken == varsym) var_declaration();
	
	if (currToken == procsym) proc_declaration();

	statement();
	errorCheck();
}

// Syntactic class for constant declaration
void const_declaraton()
{
	do
	{
		// Create a constant sym
		symbol sym = initSymbol();
		sym.kind = 1;

		getToken();

		if (currToken != identsym)
		{
			errorend(4);
			exit(0);
		}

		// Copy ident name to sym
		strcpy(sym.name, currLex.name);
		getToken();

		if (currToken != becomessym)
		{
			errorend(5);
			exit(0);
		}

		getToken();

		if (currToken != numbersym)
		{
			errorend(5);
			exit(0);
		}

		sym.val = currLex.value;

		// Add to symbol table
		enterSymbol(CONSTANT, sym.name, currLevel, sym.val);

		getToken();
	}
	while (currToken == commasym);

	if (currToken != semicolonsym)
	{
		errorend(6);
		exit(0);
	}

	getToken();
}

// Syntactic class for variable declaration
void var_declaration()
{
	do
	{
		getToken();

		if (currToken != identsym)
		{
			errorend(4);
			exit(0);
		}

		enterSymbol(VARIABLE, currLex.name, currLevel, 0);

		getToken();

	}
	while (currToken == commasym);

	if (currToken != semicolonsym)
	{
		errorend(6);
		exit(0);
	}

	getToken();
}

// Syntactic class for procedure declaration
void proc_declaration()
{
	do
	{
		getToken();

		if (currToken != identsym)
		{
			errorend(4);
			exit(0);
		}

		enterSymbol(PROCEDURE, currLex.name, currLevel, 0);

		getToken();

		if (currToken != semicolonsym)
		{
			errorend(6);
			exit(0);
		}

		getToken();

		// Go to next level before calling block, then go down a level
		upLevel();
		block();
		downLevel();

		if (currToken != semicolonsym)
		{
			errorend(6);
			exit(0);
		}

		getToken();

	}
	while(currToken == procsym);
}

// Syntactic class for a statement
void statement()
{
	errorCheck();

	if (currToken == identsym)	// ident
	{
		checkVarDeclared();
		getToken();
		if (currToken != becomessym)
		{
			errorend(2);
			exit(0);
		}

		getToken();
		checkExpressionFollows();
		expression();
		
		// ERROR If next token is not ; end ident call begin if while read write
		if(!checkValidTokenAfterStatement())
		{
			errorend(2);
			exit(0);
		}
	}

	else if (currToken == callsym) // call
	{
		getToken();
		if (currToken != identsym)
		{
			errorend(14);
			exit(0);
		}
		checkVarDeclared();
		getToken();
	}

	else if (currToken == beginsym) // begin
	{
		getToken();
		statement();

		// Begins next statement/skips over multiple ;'s
		while (currToken == semicolonsym)
		{
			getToken();
			statement();
		}

		if (currToken != endsym)
		{
			errorend(10);
			exit(0);
		}

		getToken();
	}

	else if (currToken == ifsym) // if
	{
		getToken();
		checkConditionFollows();
		condition();
		if (currToken != thensym)
		{
			errorend(9);
			exit(0);
		}
		getToken();
		statement();

		if (currToken == elsesym)
		{
			getToken();
			statement();
		}
	}

	else if (currToken == whilesym) // while
	{
		getToken();
		checkConditionFollows();
		condition();
		if (currToken != dosym)
		{
			errorend(8);
			exit(0);
		}
		getToken();
		statement();
	}

	else if (currToken == readsym) // read
	{
		getToken();
		if (currToken != identsym)
		{
			errorend(14);
			exit(0);
		}
		checkVarDeclared();
		getToken();
		statement();
	}

	else if (currToken == writesym) // write
	{
		getToken();
		checkExpressionFollows();
		expression();
		getToken();
		statement();
	}

	else // catch for null statement
	{
		return;
	}
}

// Syntactic class for condition
void condition()
{
	if (currToken == oddsym)
	{
		getToken();
		checkExpressionFollows();
		expression();
	}
	else
	{
		checkExpressionFollows();
		expression();
		if (!rel_op())
		{
			errorend(12);
			exit(0);
		}
		getToken();
		checkExpressionFollows();
		expression();
	}
}

// Check if the current token is a valid relational operator
int rel_op()
{
	return (currToken == lessym || currToken == leqsym || currToken == gtrsym ||
		currToken == geqsym || currToken == eqlsym || currToken == neqsym);
}

// Syntactic class for expression
void expression()
{
	if (currToken == plussym || currToken == minussym)
	{
		getToken();
	}

	term();

	while (currToken == plussym || currToken == minussym)
	{
		getToken();
		term();
	}
}

// Syntactic class for term
void term()
{
	checkFactorFollows();
	factor();
	while (isTermOp())
	{
		getToken();
		checkFactorFollows();
		factor();
	}
}


// Syntactic class for factor
void factor()
{
	if (currToken == identsym) // ident
	{
		checkVarDeclared();
		getToken();
	}
	else if (currToken == numbersym) // num
	{
		getToken();
	}
	else if (currToken == lparentsym) // left paren
	{
		getToken();
		checkExpressionFollows();
		expression();
		if (currToken != rparentsym)
		{
			errorend(13);
			exit(0);
		}
		getToken();
	}
	else
	{
		errorend(2);
		exit(0); // ident or number expected
	}

}

// Check that the token following a statement is valid
int checkValidTokenAfterStatement()
{
	return (currToken == semicolonsym || currToken == beginsym 
			|| currToken == endsym || currToken == identsym 
			|| currToken == callsym || currToken == ifsym 
			|| currToken == whilesym || currToken == readsym 
			|| currToken == writesym);
}

// Check that the next token starts a factor class
int checkFactorFollows()
{
	if (currToken == identsym || currToken == numbersym 
		|| currToken == lparentsym)
		return 1;
	else
	{
		error = 2;
		return 0;
	}
}

// Check that the next token starts an expression class
int checkExpressionFollows()
{
	if (currToken == plussym || currToken == minussym || currToken == identsym
		|| currToken == numbersym || currToken == lparentsym 
		|| currToken == periodsym)
		return 1;
	else
	{
		error = 2;
		return 0;
	}
	
}

// Check that the next token starts a condition class
void checkConditionFollows()
{
	if (!checkExpressionFollows() && currToken != oddsym)
	{
		errorend(11);
		exit(0);
	}
}

// Check if a variable is already declared
void checkVarDeclared()
{
	int found = 0;

	for (int i = 0; i < sym_index; i++)
	{
		if (strcmp(table[i].name, currLex.name) == 0)
			found = 1;
	}

	if (!found)
	{
		errorend(7);
		exit(0);
	}
}

// Check if the current token is a valid operator for a term
int isTermOp()
{
	return (currToken == multsym || currToken == slashsym 
			|| currToken == modsym);
}

// Move the lexicographical level up
void upLevel()
{
	currLevel++;
	prevAddress = currAddress;
	currAddress = BASE_ADDR;
}

// Move the lexicographical level down
void downLevel()
{
	currLevel--;
	currAddress = prevAddress;
}

// Check if any errors are stored in the global variable error
void errorCheck()
{
	if (error != 0)
	{
		errorend(error);
		exit(0);
	}
}

// Print an error
void errorend(int x)
{
	switch (x)
	{
		case 1:
			printf("Parser Error: Competing Symbol Declarations\n");
			break;
		case 2:
			printf("Parser Error: Unrecognized Statement Form\n");
			break;
		case 3:
			printf("Parser Error: Programs Must Close with a Period\n");
			break;
		case 4:
			printf("Parser Error: Symbols Must Be Declared with an Identifier\n");
			break;
		case 5:
			printf("Parser Error: Constants Must Be Assigned a Value at Declaration\n");
			break;
		case 6:
			printf("Parser Error: Symbol Declarations Must Be Followed By a Semicolon\n");
			break;
		case 7:
			printf("Parser Error: Undeclared Symbol\n");
			break;
		case 8:
			printf("Parser Error: while Must Be Followed By do\n");
			break;
		case 9:
			printf("Parser Error: if Must Be Followed By then\n");
			break;
		case 10:
			printf("Parser Error: begin Must Be Followed By end\n");
			break;
		case 11:
			printf("Parser Error: while and if Statements Must Contain Conditions\n");
			break;
		case 12:
			printf("Parser Error: Conditions Must Contain a Relational-Operator\n");
			break;
		case 13:
			printf("Parser Error: ( Must Be Followed By )\n");
			break;
		case 14:
			printf("Parser Error: call and read Must Be Followed By an Identifier\n");
			break;
		default:
			printf("Implementation Error: Unrecognized Error Code\n");
			break;
	}

}

// Print the symbol table
void printtable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address\n");
	printf("------------------------------------------------------\n");
	for (i = 0; i < sym_index; i++)
		printf("%4d | %11s | %5d | %5d | %5d\n", table[i].kind, table[i].name, 
				table[i].val, table[i].level, table[i].addr);
}
