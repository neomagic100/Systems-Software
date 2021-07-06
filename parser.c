#include <stdio.h>
#define MAX_SYMBOL_TABLE_SIZE 500
#define MAX_NAME 11

typedef enum {
	oddsym = 1, eqlsym, neqsym, lessym, leqsym, gtrsym, geqsym,
	modsym, multsym, slashsym, plussym, minussym,
	lparentsym, rparentsym, commasym, periodsym, semicolonsym,
	becomessym, beginsym, endsym, ifsym, thensym, elsesym,
	whilesym, dosym, callsym, writesym, readsym, constsym,
	varsym, procsym, identsym, numbersym,
} token_type;

typedef struct {
    int kind;   // const = 1, var = 2, procedures = 3
    char name[MAX_NAME + 1];
    int val;
    int level;
    int addr;
    int mark;
} symbol;

int program();
int block();
int const_declaration();
int var_declaration();
int procedure_declaration();
int statement();
int condition();
int rel_op();
int expression();
int term();
int factor();
int valid_terminal();

void initTable();
void insert_table();
void printtable();

void print_error(int);

symbol *table;
int sym_index;
int tokens[MAX_SYMBOL_TABLE_SIZE]; //FIXME - placeholder for input tokens
int token_index;
int error;

int main()
{

    return 0;
}
