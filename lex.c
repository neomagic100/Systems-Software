/*
	This is the lex skeleton for the UCF Summer 2021 Systems Software Project
	Implement the function lexanalyzer, add as many functions and global
	variables as desired, but do not alter printerror or printtokens.
	Include your name (and your partner's name) in this comment in order to
	earn the points for compiling
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define CHAR_MAX 11
#define INT_MAX 99999
#define INT_MAX_DIGITS 5
#define NORW 14 // number of reserved words
#define NUM_SYM 17 // number of symbols
#define INVALID -1

lexeme *list;
int lex_index;

//FIXME
const char symbols[NUM_SYM][3] = {"==", "<>", "<", "<=", ">", ">=", "%", "*", "/", "+", "-", "(", ")", ",", ".", ";", ":="};
const char reserved[NORW][CHAR_MAX] = {"const", "var", "procedure", "call", "if", "then", "else", "while", "do",
									"begin", "end", "read", "write", "odd"};

void printerror(int type);
void printtokens();
int isReserved(char* s);
int issymb(char c);
int isValidSymbol(char* s);
lexeme createReservedLex(char* s);
lexeme identOrKeyword(char* input);
lexeme tokenizeNum(char* input);
lexeme tokenizeSymbol(char* input);
int readComment(char* input, int* listIndex);

lexeme *lexanalyzer(char *input)
{
	list = malloc(500 * sizeof(lexeme));
	lex_index = 0;
	int listIndex = 0;

	char ch;
	char* curr;
	lexeme currLex;

	// Test: echo input
	printf("%s\n\n", input);

	while (lex_index < strlen(input))
	{
		// get first char to start analyzing
		ch = input[lex_index];

		// comment check, if ch is * and prev was /
		if (ch == '*' && list[listIndex - 1].type == slashsym) {
			if (readComment(input, &listIndex) == -1)
				return NULL;
		}

		//FIXME - remove when symbols are tokenized
		//Test symbol for comment
		else if (ch == '/') {
			lexeme currLex;
			strcpy(currLex.name, "/\0");
			currLex.type = slashsym;
			lex_index++;
			list[listIndex++] = currLex;
		}

		// If ch char is letter
		else if (isalpha(ch))
		{
			currLex = identOrKeyword(input);
			if (currLex.type == INVALID)
				return NULL;

			list[listIndex++] = currLex;
			//Test Print
			//printf("lex name: %s  lex type: %d  lex val: %d\n", currLex.name, currLex.type, currLex.value);
		}

		// If ch char is a digit
		else if (isdigit(ch))
		{
			currLex = tokenizeNum(input);
			if (currLex.type == INVALID)
				return NULL;

			list[listIndex++] = currLex;
			//Test Print
			//printf("lex name: %s  lex type: %d  lex val: %d\n", currLex.name, currLex.type, currLex.value);
		}

		// If ch is a symbol
		else if (issymb(ch))
		{
			currLex = tokenizeSymbol(input);
			if (currLex.type == INVALID)
				return NULL;

			list[listIndex++] = currLex
		}

		// If ch is ignorable character, incrememnt index
		else if (isspace(ch))
		{
			lex_index++;
		}

		else lex_index++;

	}
	printtokens();
	return list;
}

lexeme tokenizeSymbol(char* input)
{
	//TODO
}

int readComment(char* input, int* listIndex)
{
	char prevCh;
	char ch = input[lex_index];

	// Decrement list index and set type of / in list to -1
	*listIndex = *listIndex - 1;
	list[*listIndex].type = INVALID;
	strcpy(list[*listIndex].name, "INVALID");

	// Proceed through input until * and / are reached
	do
	{
		lex_index++;
		prevCh = ch;
		ch = input[lex_index];
	} while (ch != '/' || prevCh != '*' && lex_index < strlen(input));

	lex_index++;

	// neverending comment
	if (lex_index >= strlen(input))
	{
		printerror(5);
		return INVALID;
	}

	return 1;
}

lexeme identOrKeyword(char* input)
{
	lexeme currLex;
	int chCnt = 0;
	char str[CHAR_MAX + 1];
	char ch = input[lex_index];
	do
	{
		str[chCnt++] = ch; // if first is alpha, put into lexeme
		lex_index++;
		ch = input[lex_index];
	}
	while (isalnum(ch));

	// At this point, reached end of string with letter (letter | digit)*
	str[chCnt] = '\0';

	// Check to see if the string is a reserved word
	if (isReserved(str))
	{
		currLex = createReservedLex(str);
	}


	// Check to make sure its <= 11 chars
	else if (strlen(str) > 11) {
		printerror(4);
		currLex.type = INVALID;
		return currLex;
	}

	else
	{
		strcpy(currLex.name, str);
		currLex.type = identsym;
	}

	return currLex;
}

lexeme createReservedLex(char* s)
{
	lexeme res;
	strcpy(res.name, s);

	if (strcmp(s, "odd") == 0)
		res.type = oddsym;
	else if (strcmp(s, "begin") == 0)
		res.type = beginsym;
	else if (strcmp(s, "end") == 0)
		res.type = endsym;
	else if (strcmp(s, "if") == 0)
		res.type = ifsym;
	else if (strcmp(s, "then") == 0)
		res.type = thensym;
	else if (strcmp(s, "else") == 0)
		res.type = elsesym;
	else if (strcmp(s, "while") == 0)
		res.type = whilesym;
	else if (strcmp(s, "do") == 0)
		res.type = dosym;
	else if (strcmp(s, "call") == 0)
		res.type = callsym;
	else if (strcmp(s, "write") == 0)
		res.type = writesym;
	else if (strcmp(s, "read") == 0)
		res.type = readsym;
	else if (strcmp(s, "const") == 0)
		res.type = constsym;
	else if (strcmp(s, "var") == 0)
		res.type = varsym;
	else if (strcmp(s, "procedure") == 0)
		res.type = procsym;

	return res;
}

int isReserved(char* s)
{
	for (int i = 0; i < NORW; i++)
	{
		if (strcmp(s, reserved[i]) == 0)
			return 1;
	}

	return 0;
}

lexeme tokenizeNum(char* input)
{
	lexeme currLex;
	int digitCnt = 0;
	int val = 0;
	char ch = input[lex_index];
	char str[INT_MAX_DIGITS+1];

	// Read in the number until ch is not a digit
	do
	{
		str[digitCnt++] = ch;
		ch = input[++lex_index];

		// Print error if more than 6 digits
		if (digitCnt > 5)
		{
			printerror(3);
			currLex.type = INVALID;
			return currLex;
		}

	} while (isdigit(ch));

	// Print error if digit is immediately followed by letter
	if (isalpha(ch))
	{
		printerror(2);
		currLex.type = INVALID;
		return currLex;
	}

	// Processes and tokenizes number
	currLex.type = numbersym;
	val = (str[0] - '0');

	for (int i = 1; i < digitCnt; i++)
	{
		val = (val*10) + (str[i] - '0');
	}

	currLex.value = val;

	return currLex;
}



// Determine if a single character can be part of a valid symbol
int issymb(char c)
{
	char validSyms[] = {'=', '<', '>', '%', '*', '/', '+', '-', '(', ')', ',', '.', ';', ':'};
	for (int i = 0; i < strlen(validSyms); i++)
	{
		if (c == validSyms[i])
			return 1;
	}

	return 0;
}

int isValidSymbol(char* s)
{
	for (int i = 0; i < NUM_SYM; i++)
	{
		if (strcmp(s, symbols[i]) == 0)
			return 1;
	}

	return 0;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "<>", neqsym);
				break;
			case lessym:
				printf("%11s\t%d", "<", lessym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case slashsym:
				printf("%11s\t%d", "/", slashsym);
				break;
			case plussym:
				printf("%11s\t%d", "+", plussym);
				break;
			case minussym:
				printf("%11s\t%d", "-", minussym);
				break;
			case lparentsym:
				printf("%11s\t%d", "(", lparentsym);
				break;
			case rparentsym:
				printf("%11s\t%d", ")", rparentsym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case becomessym:
				printf("%11s\t%d", ":=", becomessym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else if (type == 5)
		printf("Lexical Analyzer Error: Neverending Comment\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}
