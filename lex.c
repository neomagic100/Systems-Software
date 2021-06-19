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
#define INT_MAX_DIGITS 5
#define NORW 14 // number of reserved words
#define NUM_SYM 17 // number of symbols
#define INVALID -1

lexeme *list;
int lex_index;
int input_char_count;

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
int readComment(char* input);

lexeme *lexanalyzer(char *input)
{
	list = malloc(500 * sizeof(lexeme));
	lex_index = 0;
	input_char_count = 0;
	char ch;
	lexeme currLex; // current lexeme being read

	// Tokenize input until all characters are read
	while (input_char_count < strlen(input))
	{
		// get first char to start analyzing
		ch = input[input_char_count];

		// break if read null character (reached end of input string)
		if (ch == '\0')
			break;

		// Comment check, if ch is * and prev was /
		if (ch == '*' && list[lex_index - 1].type == slashsym) {
			if (readComment(input) == -1)
				return NULL;
		}

		// If ch char is letter
		else if (isalpha(ch))
		{
			currLex = identOrKeyword(input);
			if (currLex.type == INVALID)
				return NULL;

			list[lex_index++] = currLex;
		}

		// If ch char is a digit
		else if (isdigit(ch))
		{
			currLex = tokenizeNum(input);
			if (currLex.type == INVALID)
				return NULL;

			list[lex_index++] = currLex;
		}

		// If ch is a symbol
		else if (issymb(ch))
		{
			currLex = tokenizeSymbol(input);
			if (currLex.type == INVALID)
			{
				printerror(1);
				return NULL;
			}

			list[lex_index++] = currLex;
		}

		// If ch is ignorable character (whitespace, tab, or newline), incrememnt input_char_count to skip it
		else if (isspace(ch))
		{
			input_char_count++;
		}

		else // Catch for non-conforming cases
		{
			printf("Tokenize error.\n");
			input_char_count++;
		}

	}

	printtokens();
	return list;
}

// Tokenize a symbol read from input
lexeme tokenizeSymbol(char* input)
{
	lexeme currLex;
	char str[3];
	str[0] = input[input_char_count];
	str[1] = input[input_char_count + 1];
	str[2] = '\0';

	switch (str[0]) {
		case '=':
			if (str[1] != '=')
			{
				currLex.type = INVALID;
				input_char_count++;
				break;
			}

			currLex.type = eqlsym;
			input_char_count += 2;
			break;

		case '<':
			if (str[1] == '>')
			{
				currLex.type = neqsym;
				input_char_count += 2;
				break;
			}
			else if (str[1] == '=')
			{
				currLex.type = leqsym;
				input_char_count += 2;
				break;
			}
			
			currLex.type = lessym;
			input_char_count++;
			break;

		case '>':
			if (str[1] == '=')
			{
				currLex.type = geqsym;
				input_char_count += 2;
				break;
			}

			currLex.type = gtrsym;
			input_char_count++;
			break;

		case '%':
			currLex.type = modsym;
			input_char_count++;
			break;

		case '*':
			currLex.type = multsym;
			input_char_count++;
			break;

		case '/':
			currLex.type = slashsym;
			input_char_count++;
			break;

		case '+':
			currLex.type = plussym;
			input_char_count++;
			break;

		case '-':
			currLex.type = minussym;
			input_char_count++;
			break;

		case '(':
			currLex.type = lparentsym;
			input_char_count++;
			break;

		case ')':
			currLex.type = rparentsym;
			input_char_count++;
			break;

		case ',':
			currLex.type = commasym;
			input_char_count++;
			break;

		case '.':
			currLex.type = periodsym;
			input_char_count++;
			break;

		case ';':
			currLex.type = semicolonsym;
			input_char_count++;
			break;

		case ':':
			if (str[1] != '=')
			{
				currLex.type = INVALID;
				input_char_count++;
				break;
			}

			currLex.type = becomessym;
			input_char_count += 2;
			break;

		default:
			currLex.type = INVALID;
			input_char_count++;
			break;
	}

	return currLex;
}

// Read a comment to ignore
int readComment(char* input)
{
	char prevCh;
	char ch = input[input_char_count];

	// Decrement list index and set type of / in list to -1 to remove lexeme of /
	lex_index = lex_index - 1;
	list[lex_index].type = INVALID;
	strcpy(list[lex_index].name, "INVALID");

	// Proceed through input until * and / are reached
	do
	{
		input_char_count++;
		prevCh = ch;
		ch = input[input_char_count];
	} while (ch != '/' || prevCh != '*' && input_char_count < strlen(input));

	input_char_count++;

	// Make sure its not a neverending comment
	if (input_char_count >= strlen(input))
	{
		printerror(5);
		return INVALID;
	}

	return 1;
}

// Read in an expression L (D | L)* for an identifier or L+ for keyword
lexeme identOrKeyword(char* input)
{
	lexeme currLex;
	int chCnt = 0;
	char str[CHAR_MAX + 1];
	char ch = input[input_char_count];
	do
	{
		str[chCnt++] = ch; // if first is alpha, put into lexeme
		input_char_count++;
		ch = input[input_char_count];
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

	// Current string read must be an identifier
	else
	{
		strcpy(currLex.name, str);
		currLex.type = identsym;
	}

	return currLex;
}

// Create a lexeme for a keyword, given input string s
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

// Determine if a string is a reserved keyword
int isReserved(char* s)
{
	for (int i = 0; i < NORW; i++)
	{
		if (strcmp(s, reserved[i]) == 0)
			return 1;
	}

	return 0;
}

// Create a token for a lexeme of an integer
lexeme tokenizeNum(char* input)
{
	lexeme currLex;
	int digitCnt = 0;
	int val = 0;
	char ch = input[input_char_count];
	char str[INT_MAX_DIGITS+1];

	// Read in the number until ch is not a digit
	do
	{
		str[digitCnt++] = ch;
		ch = input[++input_char_count];

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
	const char validSyms[] = {'=', '<', '>', '%', '*', '/', '+', '-', '(', ')', ',', '.', ';', ':'};
	for (int i = 0; i < strlen(validSyms); i++)
	{
		if (c == validSyms[i])
			return 1;
	}

	return 0;
}

// Determine if a string is a valid symbol
int isValidSymbol(char* s)
{
	for (int i = 0; i < NUM_SYM; i++)
	{
		if (strcmp(s, symbols[i]) == 0)
			return 1;
	}

	return 0;
}

// Print all tokens in lexeme table
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
	list[lex_index].type = -1;
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
