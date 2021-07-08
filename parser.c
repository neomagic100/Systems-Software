#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#define CONST_KIND 1
#define VAR_KIND 2
#define PROC_KIND 3

symbol *table;
int sym_index;
int error;

void printtable();
void errorend(int x);

// Added Global Vars
lexeme* lexList;
int lex_index;
lexeme currentToken;

// Added functions
void getNextToken()
{
	currentToken = lexList[lex_index++];
}



int peekNext()
{
	return lexList[lex_index + 1].type;
}

int checkNextTokenValid(int sym)
{
	return (peekNext() == sym);
}

int expectStatement() {
	return (peekNext() == identsym || peekNext() == callsym || peekNext() == beginsym || peekNext() == ifsym ||
			peekNext() == whilesym || peekNext() == readsym || peekNext() == writesym); // || empty

}

int program_Parse();
int block_Parse();
int const_declaration_Parse();
int var_declaration_Parse();
int procedure_declaration_Parse();
int statement_Parse();
int condition_Parse();
int rel_op_Parse();
int expression_Parse();
int term_Parse();
int factor_Parse();
int validTerminal();
symbol createToken(int, char*, int, int, int);
symbol readAssignment(int);

symbol *parse(lexeme *input)
{
	printf("Parse start");
	table = malloc(1000 * sizeof(symbol));
	sym_index = 0;
	error = 0;

	// Added Code
	lexList = input;
	lex_index = 0;

	program_Parse();

	// end added code

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

/* Added Code */

int program_Parse()
{
	// Parse block
	block_Parse();

	// Follow with period
	if (currentToken.type != periodsym)
	{
		errorend(3);
		error = 3;
		return -1;
	}

	return 0;

}

int block_Parse()
{

	if (currentToken.type == constsym)
	{
		getNextToken();
		const_declaration_Parse();
	}

	if (currentToken.type == varsym)
	{
		getNextToken();
		//var_declaration_Parse();
	}

	if (currentToken.type == procsym)
	{
		getNextToken();
		//procedure_declaration_Parse();
	}

	while (expectStatement())
		statement_Parse();

	return 0;
}

int statement_Parse()
{
	//TODO
	return 0;
}

symbol readAssignment(int kind)
{
	symbol sym;

	sym.kind = kind;

	if (currentToken.type != identsym)
	{
		errorend(4);
		error = 4;
		sym.kind = -1;
	}

	strcpy(sym.name, currentToken.name);

	getNextToken();

	if (currentToken.type != becomessym)
	{
		errorend(5);
		error = 5;
		sym.kind = -1;
	}

	getNextToken();

	if (currentToken.type != numbersym)
	{
		errorend(5);
		error = 5;
		sym.kind = -1;
	}

	sym.val = currentToken.value;

	getNextToken();

	return sym;

}

int const_declaration_Parse()
{
	symbol sym;
	int isNextAssignment = 0;

	do {

		sym = readAssignment(CONST_KIND);

		if (sym.kind == -1)
			return -1;


		table[sym_index++] = sym;
		if (peekNext() == commasym) {
			lex_index += 2;
			isNextAssignment = 1;
		}
	}
	while (isNextAssignment);

	if (currentToken.type != semicolonsym)
	{
		errorend(6);
		error = 6;
		return -1;
	}

	return 0;
}


// Use or Don't use. Just Brainstorming
symbol createToken(int kind, char* name, int val, int level, int addr)
{
	symbol retSym;
	retSym.kind = kind;
	retSym.mark = 0;

	if (kind == 1)
	{
		strcpy(retSym.name, name);
		retSym.val = val;
		retSym.level = level;
	}
	else if (kind == 2)
	{
		strcpy(retSym.name, name);
		retSym.level = level;
		retSym.addr = addr;
	}
	else if (kind == 3)
	{
		strcpy(retSym.name, name);
		retSym.level = level;
	}
	else
	{
		retSym.mark = -1;
	}

	return retSym;
}

/* End Added Code */

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
