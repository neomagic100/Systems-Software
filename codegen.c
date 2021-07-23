/*
	Author:
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

// Global variables and pointers
symbol *sym_table;
lexeme *lex_tokens;
lexeme currLex;
int sym_index;
int currToken;
int token_index;
int currLevel;
int currAddress;
int prevAddress;

instruction *code;
int code_index;

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

void getToken();
void printcode();

instruction *generate_code(lexeme *tokens, symbol *symbols)
{
	code = malloc(500 * sizeof(instruction));
	sym_index = code_index = 0;
	token_index = -1;
	currLevel = 0;
	currAddress = prevAddress = 3;
	sym_table = symbols;
	lex_tokens = tokens;


	printcode();
	return code;
}

// Update the global varables to get the next token
void getToken()
{
	currToken = tokens[++token_index].type;
	currLex = tokens[token_index];
}

void program()
{

}

void block()
{

}

void statement()
{

}

void const_declaraton()
{

}

void var_declaration()
{

}

void proc_declaration()
{

}

void condition()
{
	// Gen code for comparisons
}

int rel_op()
{

}

void expression()
{
	// Gen code for + or -
}

void term()
{
	// Gen code for term op here
}

void factor()
{

}

void printcode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < code_index; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
}
