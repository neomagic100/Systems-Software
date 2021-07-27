/*********************************************************************
 *  Assignment 4: Intermediate Code Generator                        *
 *  COP 3402 Summer 2021                                             *
 *  Authors: Willow Maddox                                           *
 *           Michael Bernhardt                                       *
 *********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

// Enumeration for OP, OPR, and SYS codes
typedef enum op_code {
	LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SYS,
	RTN = 0, NEG, ADD, SUB, MUL, DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ,
	WRITE = 1, READ, HALT,
} op_code;

// Global variables and pointers
symbol *sym_table;
lexeme *lex_tokens;
lexeme currLex;
int sym_index;
int currToken;
int token_index;
int currLevel;
int sym_table_size;
int currProc; // tracks sym index of current procedure for CAL

instruction *code;
int code_index;

// Functions
void program();
void block();
void statement();
void const_declaration();
int var_declaration();
void proc_declaration();
void condition();
void expression();
void term();
void factor();
int genCode(int, int, int);
int findToken(char*);
void getSymTableSize();
int isTermOp();
void markProcVars();
void getToken();
void printcode();

instruction *generate_code(lexeme *tokens, symbol *symbols)
{
	code = malloc(500 * sizeof(instruction));
	currProc = 0;
	sym_index = code_index = 0;
	token_index = -1;
	currLevel = -1; // Starting at -1 so main block starts at 0
	sym_table = symbols;
	lex_tokens = tokens;
	getSymTableSize();
	program();

	printcode();
	return code;
}

// Emit a line of assembly code
int genCode(int op, int l, int m)
{
	int addr;
	instruction currInstr;

	currInstr.opcode = op;
	currInstr.l = l;
	currInstr.m = m;

	code[code_index++] = currInstr;

	return addr;
}

// Calculate the size of the symbol table
void getSymTableSize()
{
	int tableSize = 0;
	for (int i = 0; i < 500; i++)
	{
		if (sym_table[i].kind == 1 || sym_table[i].kind == 2 || sym_table[i].kind == 3) tableSize++;
		else break;
	}

	sym_table_size = tableSize;
}

// Find token in symbol table
// Return index if found, 0 if not
int findToken(char* ident)
{
	for (int i = sym_table_size - 1; i >= 0; i--)
	{
		// Need to match name has to not be marked
		if (strcmp(sym_table[i].name, ident) == 0
				&& currLevel >= sym_table[i].level
				&& sym_table[i].mark == 0)
			return i;
	}

	return 0;
}

void markProcVars()
{
	int idx = sym_index - 1;

	// from last variable in procedure to procedure symbol
	while (sym_table[idx].kind != 3 && idx > 0)
		sym_table[idx--].mark = 1;
}

// Update the global varables to get the next token
void getToken()
{
	currToken = lex_tokens[++token_index].type;
	currLex = lex_tokens[token_index];
}

// Syntactic class for constant declaration
void const_declaration()
{
	do
	{
		getToken(); // ident

		// Unmark constant
		sym_table[sym_index++].mark = 0;

		getToken(); // :=
		getToken(); // number
		getToken(); // comma or semicolon
	}
	while (currToken == commasym);

	getToken(); // get next token
}

// Syntactic class for variable declaration
int var_declaration()
{
	int numVarsDeclared = 0;

	do
	{
		getToken(); // ident

		sym_table[sym_index++].mark = 0; // Unmark variable

		getToken(); // comma or semicolon

		numVarsDeclared++; // count how many variables are declared
	}
	while (currToken == commasym);

	getToken(); // get next token

	return numVarsDeclared;
}

// Syntactic class for procedure declaration
void proc_declaration()
{
	do
	{
		getToken(); // ident

		sym_table[sym_index++].mark = 0; // Unmark procedure

		getToken(); // semicolon
		getToken(); // get next token

		// Go to next level before calling block, then go down a level

		sym_table[sym_index - 1].val = code_index + 1;
		block(); // levels adjusted in block

		getToken(); // get next token
	}
	while(currToken == procsym);
}

// Syntactic class for program
void program()
{
	genCode(JMP, 0, 0); // First instruction to jump to main procedure
	sym_table[sym_index++].mark = 0;
	getToken();
	block();
}

// Syntactic class for block
void block()
{
	currLevel++;
	int space = 3; // Start with 3 spaces for AR

	if (currToken == constsym) const_declaration();
	if (currToken == varsym) space += var_declaration();
	if (currToken == procsym) proc_declaration();
	
	// Set first Jump to main
	if (currLevel == 0) code[0].m = code_index * 3;
	
	// Store the code_index in the value field of procedure symbol in table
	sym_table[sym_index].val = code_index;
	
	genCode(INC, 0, space);
	statement();

	// mark variables of procedure
	markProcVars();

	// If the current lexicographical level is greater than 0, return. Otherwise halt
	if (currLevel > 0)
	{
		genCode(OPR, 0, RTN);
		currLevel--;
	}
	else genCode(SYS, 0, HALT);

	currProc = 0;
}

// Syntactic class for statement
void statement()
{
	if (currToken == identsym)	// ident
	{
		int identToStoreIdx = findToken(currLex.name);
		getToken(); // :=
		getToken(); // get next token

		expression();

		// Store assignment to variable
		genCode(STO, currLevel - sym_table[identToStoreIdx].level, sym_table[identToStoreIdx].addr);
	}

	else if (currToken == callsym) // call
	{ 
		getToken(); // get proc ident
		int procSymIdx = findToken(currLex.name);

		// TODO: Figure out how to track the current procedure
		// sym_table[currProc+1].val = code_index;

		genCode(CAL, currLevel - sym_table[procSymIdx].level, sym_table[procSymIdx].val);

		getToken();
	}

	else if (currToken == beginsym) // begin
	{
		getToken();
		statement();

		while (currToken == semicolonsym)
		{
			getToken();
			statement();
		}

		getToken();
	}

	else if (currToken == ifsym) // if
	{
		int jpcIdx, jmpIdx;
		getToken();
		condition();

		// Store index of where the JPC isntruction is going to be in symbol table
		jpcIdx = code_index;
		genCode(JPC, 0, jpcIdx);

		getToken(); // next token after then
		statement();

		if (currToken == elsesym)
		{
			getToken();
			jmpIdx = code_index; // Index where JMP is
			genCode(JMP, 0, jmpIdx);
			code[jpcIdx].m = code_index * 3; // Fix jmp and jpc indices
			statement();
			code[jmpIdx].m = code_index * 3;
		}
		else
		{
			code[jpcIdx].m = code_index * 3;
		}
	}

	else if (currToken == whilesym) // while
	{
		int jpcIdx, jmpIdx;

		getToken(); // ident or num
		jmpIdx = code_index;
		condition();
		getToken();	// do
		jpcIdx = code_index;
		genCode(JPC, 0, jpcIdx);
		statement();
		genCode(JMP, 0, jmpIdx * 3);
		code[jpcIdx].m = code_index * 3;
	}

	else if (currToken == readsym) // read
	{
		getToken();
		int identToStoreIdx = findToken(currLex.name);

		getToken();
		
		// Read from input
		genCode(SYS, 0, READ);

		// Store value read to ident
		genCode(STO, currLevel - sym_table[identToStoreIdx].level, sym_table[identToStoreIdx].addr);

		statement();
	}

	else if (currToken == writesym) // write
	{
		getToken();

		expression();

		// Write to output
		genCode(SYS, 0, WRITE);

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
		expression();
		genCode(OPR, 0, ODD);
	}
	else
	{
		expression();
		int relop = currToken; // store conditional operation
	
		getToken(); // get token after relop
		expression();

		// After both sides of expression generated, generate code for for comparison
		switch(relop) {
			case eqlsym:
				genCode(OPR, 0, EQL);
				break;
			case neqsym:
				genCode(OPR, 0, NEQ);
				break;
			case lessym:
				genCode(OPR, 0, LSS);
				break;
			case leqsym:
				genCode(OPR, 0, LEQ);
				break;
			case gtrsym:
				genCode(OPR, 0, GTR);
				break;
			case geqsym:
				genCode(OPR, 0, GEQ);
				break;
			default:
				break;
		}
	}
}

// Syntactic class for expression
void expression()
{
	int minus = (currToken == minussym); // Store if first term has minus
	if (currToken == plussym || currToken == minussym)
		getToken(); // number (either ident, number, or "("expression")")

	term();

	if (minus)
		genCode(OPR, 0, NEG);

	while (currToken == plussym || currToken == minussym)
	{
		int currOperation = currToken;
		getToken();

		term();

		if (currOperation == plussym)
			genCode(OPR, 0, ADD);
		else if (currOperation == minussym)
			genCode(OPR, 0, SUB);
	}
}

// Syntactic class for term
void term()
{
	factor();
	while (isTermOp())
	{
		int currOperation = currToken;
		getToken();

		factor();

		if (currOperation == multsym)
			genCode(OPR, 0, MUL);
		else if (currOperation == slashsym)
			genCode(OPR, 0, DIV);
		else if (currOperation == modsym)
			genCode(OPR, 0, MOD);
	}
}

// Syntactic class for factor
void factor()
{
	if (currToken == identsym)  // ident	
	{
		// ident used, find var or const in symbol table
		int varIdx = findToken(currLex.name);
		symbol currSym = sym_table[varIdx];
		if (currSym.kind == 2)
			genCode(LOD, currLevel - currSym.level, currSym.addr);
		else
			genCode(LIT, 0, currSym.val);
		
		getToken();
	}

	else if (currToken == numbersym) // num
	{
		genCode(LIT, 0, currLex.value);
		getToken();
	}

	else if (currToken == lparentsym) // left paren
	{
		getToken();
		expression();
		getToken();
	}
}

// Check if the current token is a valid operator for a term
int isTermOp()
{
	return (currToken == multsym || currToken == slashsym
			|| currToken == modsym);
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
