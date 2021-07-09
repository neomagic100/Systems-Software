#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#define TABLE_SIZE 1000
#define CONSTANT 1
#define VARIABLE 2
#define PROCEDURE 3
#define MAX_CHARS 12

symbol *table;
int sym_index;
int num_symbols; // may be redundant
int error;

void program();
void block();
void statement();
void const_declaraton();
void var_declaration();
void proc_declaration();
void condition();
void rel_op();
void expression();
void term();
void factor();

symbol initSymbol();
void enterSymbol(int, char*, int, int);

void getToken();
int currToken;
lexeme currLex;
int token_index;
lexeme tokens*;

int currLevel;

void printtable();
void errorend(int x);

symbol *parse(lexeme *input)
{
	table = malloc(TABLE_SIZE * sizeof(symbol));
	sym_index = 0;
	error = 0;
	num_symbols = 0;
	tokens = input;
	token_index = 0;
	currLevel = 0;

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
	currToken = tokens[token_index].type;
	currLex = tokens[token_index];
	token_index++
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
	for (int i = 0; i < num_symbols; i++)
	{
		if (strcmp(table[i].name, name) == 0)
		{
			// print error
		}
	}

	symbol s;
	strcpy(s.name, name);
	s.kind = type;
	s.level = level;

	if (type == CONSTANT)
	{
		s.val = valOrAddr;
	}
	else if (type == VARIABLE)
	{
		// computer addr
		s.addr = valOrAddr;
	}

	table[sym_index] = s;
	sym_index++;
	num_symbols++;
}

void program()
{
	getToken();
	block();

	// Make sure period at end of program
	if (currToken != periodsym)
	{
		errorend(3);
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
}

void const_declaraton()
{

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
			error = 4;
			return;
		}

		strcpy(name, currLex.name);

		enterSymbol(VARIABLE, name, currLevel, 0);

		getToken();

	}
	while (currToken != commasym);

	if (currToken != semicolonsym)
	{
		errorend(6);
		error = 6;
		return;
	}
}

void proc_declaration()
{
	do
	{
		getToken();

		if (currToken != identsym)
		{
			errorend(4);
			error = 4;
			return;
		}

		
	}
	while(currToken == procsym);
}

void statement()
{

}

void condition()
{

}

void rel_op()
{

}

void expression()
{

}

void term()
{

}

void factor()
{

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
		printf("%4d | %11s | %5d | %5d\n", table[i].kind, table[i].name, table[i].value, table[i].level, table[i].addr);
}
