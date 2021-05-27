#include <stdio.h>
#include <stdlib.h>
#define MAX_PAS_LENGTH 500
//#define MAX_INSTRUCTIONS 24

typedef struct instruction_registry {
    int op;
    int l;
    int m;
} iReg;

int PC = 0,
    BP = 0,
    SP = 0,
    Halt = 1;

int pas[MAX_PAS_LENGTH];

int main(int argc, char **argv) {
    // Get input file from command line
    FILE* inputFile;
    inputFile = fopen(argv[1], "r");

    // Initialize stack
    for (int i = 0; i < MAX_PAS_LENGTH; i++)
        pas[i] = 0;

    // Declare instruction registry and initialize fields to 0
    iReg ir;
    ir.op = ir.l = ir.m = 0;



    // FETCH


    // EXECUTE

    return 0;
}

/**********************************************/
/*		Find base L levels down		 */
/*							 */
/**********************************************/
 
int base(int L)
{
	int arb = BP;	// arb = activation record base
	while (L > 0)     //find base L levels down
	{
		arb = pas[arb];
		L--;
	}
	return arb;
}
