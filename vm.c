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
    FILE* inputFile, *outFile;
    int temp;

    // Initialize stack
    for (int i = 0; i < MAX_PAS_LENGTH; i++)
        pas[i] = 0;

    // Declare instruction registry and initialize fields to 0
    iReg ir;
    ir.op = ir.l = ir.m = 0;

    // Read file into text segment
    inputFile = fopen(argv[1], "r");
    
    while (fscanf(inputFile, "%d", &pas[SP]) != -1) {
        fscanf(inputFile, " %d %d\n", &pas[SP + 1], &pas[SP + 2]);
        SP += 3;
    }
    fclose(inputFile);

    // Set BP to next open index and SP to last index of text segment
    BP = SP;
    SP--;

    // Open output file to print results and print initial values
    outFile = fopen("output.txt", "w");
    fprintf(outFile, "                PC\tBP\tSP\tStack\n");
    fprintf(outFile, "Initial values: %2d\t%2d\t%2d\n", PC, BP, SP);


    // Instruction Loop
    while (Halt == 1) {
        /* FETCH */

        // Get Instruction
        ir.op = pas[PC];
        ir.l = pas[PC + 1];
        ir.m = pas[PC + 2];
        PC += 3;

        /* EXECUTE */

        // Determine action based on op code
        switch(ir.op) {
            case 1:
                //TODO
                break;
            case 2:
                //TODO
                break;
            case 3:
                //TODO
                break;
            case 4:
                //TODO
                break;
            case 5:
                //TODO
                break;
            case 6:
                //TODO
                break;
            case 7:
                //TODO
                break;
            case 8:
                //TODO
                break;
            case 9:
                //TODO
                break;
            default:
                //TODO
                break;
        } // end op switch


    } // end instruction while loop


    
    fclose(outFile);
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
