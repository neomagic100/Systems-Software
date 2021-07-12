#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#define TABLE_SIZE 1000
#define CONSTANT 1
#define VARIABLE 2
#define PROCEDURE 3
#define MAX_CHARS 12
#define BASE_ADDR 3

symbol *table;
int sym_index;
int error;

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
void enterSymbol(int, char*, int, int);
int checkSymbolTable(char *s);

void getToken();
int currToken;
lexeme currLex;
int token_index;
lexeme *tokens;

int currLevel;
int currAddress;
int prevAddress;

void upLevel();
void downLevel();

void printtable();
void errorend(int x);

symbol *parse(lexeme *input)
{
	table = malloc(TABLE_SIZE * sizeof(symbol));
	sym_index = 0;
	error = 0;
	tokens = input;
	token_index = -1;
	currLevel = 0;
	currAddress = prevAddress = BASE_ADDR;

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

void getToken()
{
	currToken = tokens[++token_index].type;
	currLex = tokens[token_index];
}

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

void enterSymbol(int type, char* name, int level, int valOrAddr)
{
	// check for conflicting idents
	for (int i = 0; i < sym_index; i++)
	{
		if (strcmp(table[i].name, name) == 0 && table[i].level == level)
		{
			errorend(1);
			exit(0);
		}
	}

	symbol s;
	s.mark = 0;
	strcpy(s.name, name);
	s.kind = type;
	s.level = level;


	if (type == CONSTANT)
	{
		s.val = valOrAddr;
	}
	else
	{
		s.val = 0;
	}
	if (type == VARIABLE)
	{
		s.addr = currAddress++;
	}
	else
	{
		s.addr = 0;
	}

	table[sym_index] = s;
	sym_index++;
}

int checkSymbolTable(char *s)
{
	for (int i = 0; i <= token_index; i++)
	{
		if (strcmp(s, table[i].name) == 0)
			return i;
	}

	return -1;
}

void program()
{
	enterSymbol(PROCEDURE, "main", 0, 0);
	getToken();
	block();

	// Make sure period at end of program
	if (currToken != periodsym)
	{
		errorend(3);
		exit(0);
	}
}

void block()
{
	if (currToken == constsym)
	{
		const_declaraton();
	}
	if (currToken == varsym)
	{
		var_declaration();
	}
	if (currToken == procsym)
	{
		proc_declaration();
	}

	statement();
	errorCheck();
}

void const_declaraton()
{
	do
	{
		symbol sym = initSymbol();
		sym.kind = 1;

		getToken();

		if (currToken != identsym)
		{
			errorend(4);
			exit(0);
		}

		// Checks that identifier name isn't already in use
		/*if (checkSymbolTable(currLex.name) != -1)
		{
			errorend(1);
			exit(0);
		}*/

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

		// add to symbol table
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

void var_declaration()
{
	do
	{
		char name[MAX_CHARS];

		getToken();

		if (currToken != identsym)
		{
			errorend(4);
			exit(0);
		}

		strcpy(name, currLex.name);

		enterSymbol(VARIABLE, name, currLevel, 0);

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

void statement()
{
	errorCheck();

	if (currToken == identsym)
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

	else if (currToken == callsym)
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

	else if (currToken == beginsym)
	{
		getToken();
		statement();

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

	else if (currToken == ifsym)
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

	else if (currToken == whilesym)
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

	else if (currToken == readsym)
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

	else if (currToken == writesym)
	{
		getToken();
		checkExpressionFollows();
		expression();
		getToken();
		statement();
	}

	else 
	{
		return;
	}
}

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

int rel_op()
{
	return (currToken == lessym || currToken == leqsym || currToken == gtrsym ||
		currToken == geqsym || currToken == eqlsym || currToken == neqsym);
}

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

void factor()
{
	if (currToken == identsym)
	{
		checkVarDeclared();
		getToken();
	}
	else if (currToken == numbersym)
	{
		getToken();
	}
	else if (currToken == lparentsym)
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

// ERROR If next token is not ; end ident call begin if while read write 
int checkValidTokenAfterStatement()
{
	return (currToken == semicolonsym || currToken == beginsym || currToken == endsym || currToken == identsym || currToken == callsym
		|| currToken == ifsym || currToken == whilesym || currToken == readsym || currToken == writesym);
}

int checkFactorFollows()
{
	if (currToken == identsym || currToken == numbersym || currToken == lparentsym)
		return 1;
	else
	{
		error = 2;
		return 0;
	}
}

int checkExpressionFollows()
{
	if (currToken == plussym || currToken == minussym || currToken == identsym || 
		currToken == numbersym || currToken == lparentsym)
		return 1;
	else
	{
		error = 2;
		return 0;
	}
	
}

void checkConditionFollows()
{
	if (!checkExpressionFollows() && currToken != oddsym)
	{
		errorend(11);
		exit(0);
	}
}

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

int isTermOp()
{
	return (currToken == multsym || currToken == slashsym || currToken == modsym);
}

void upLevel()
{
	currLevel++;
	prevAddress = currAddress;
	currAddress = BASE_ADDR;
}

void downLevel()
{
	currLevel--;
	currAddress = prevAddress;
}

void errorCheck()
{
	if (error != 0)
	{
		errorend(error);
		exit(0);
	}
}

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

void printtable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address\n");
	printf("------------------------------------------------------\n");
	for (i = 0; i < sym_index; i++)
		printf("%4d | %11s | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr);
}
