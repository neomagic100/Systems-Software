#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#define TABLE_SIZE 1000

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
void addSymbolToTable(symbol s);

int getToken();
int currToken;
int token_index;
lexeme tokens*;

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

int getToken()
{
	return tokens[token_index++].type;
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

void addSymbolToTable(symbol s)
{
	// check for conflicting idents
	for (int i = 0; i < num_symbols; i++)
	{
		if (strcmp(table[i].name, s.name) == 0)
		{
			// print error
		}
	}


	table[sym_index] = s;
	sym_index++;
	num_symbols++;
}

void program()
{
	currToken = getToken();
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

	}
	if (currToken == procsym)
	{

	}

	statement();
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

		}

		strcpy(sym.name, tokens[token_index].name)

		getToken();
		if (currToken != becomessym)
		{

		}
		getToken();
		if (currToken != numbersym)
		{

		}

		sym.val = tokens[token_index].value;

		// add to symbol table
		addSymbolToTable(sym);

		getToken();


	}
	while (currToken != commasym)

	if (currToken != semicolonsym)
	{

	}
	getToken();
}

void var_declaration()
{

}

void proc_declaration()
{

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
