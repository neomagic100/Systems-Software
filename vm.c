#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PAS_LENGTH 500


int base(int);

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
        char opStr[4];
        int initialPC = PC;

        /* FETCH */

        // Get Instruction
        ir.op = pas[PC];
        ir.l = pas[PC + 1];
        ir.m = pas[PC + 2];
        PC += 3;

        /* EXECUTE */

        // Determine action based on op code
        switch(ir.op) {
            case 1: // LIT
                SP++;
                pas[SP] = ir.m;
                strcpy(opStr, "LIT");
                break;

            case 2: // OPR
                // Determine action based on M field on instruction
                switch (ir.m) {
                    case 0: // RTN
                        //TODO
                        break;

                    case 1: // NEG
                        //TODO
                        break;

                    case 2: // ADD
                        //TODO
                        break;

                    case 3: // SUB
                        //TODO
                        break;
                    
                    case 4: // MUL
                        //TODO
                        break;

                    case 5: // DIV
                        //TODO
                        break;

                    case 6: // ODD
                        //TODO
                        break;

                    case 7: // MOD
                        //TODO
                        break;
                    
                    case 8: // EQL
                        //TODO
                        break;

                    case 9: // NEQ
                        //TODO
                        break;

                    case 10: // LSS
                        //TODO
                        break;

                    case 11: // LEQ
                        //TODO
                        break;
                    
                    case 12: // GTR
                        //TODO
                        break;

                    case 13: // GEQ
                        //TODO
                        break;

                    default:
                        // FIXME
                        printf("Error in OPR case switch\n");
                        break;
                } // End OPR switch
                strcpy(opStr, "OPR");
                break;

            case 3: // LOD
                SP++;
                pas[SP] = pas[base(ir.l) + ir.m];
                strcpy(opStr, "LOD");
                break;

            case 4: // STO
                pas[base(ir.l) + ir.m] = pas[SP];
                SP--;
                strcpy(opStr, "STO");
                break;

            case 5: // CAL
                pas[SP + 1] = base(ir.l); // static link
                pas[SP + 2] = BP;         // dynamic link
                pas[SP + 3] = PC;         // return address
                BP = SP + 1;
                PC = ir.m;
                strcpy(opStr, "CAL");
                break;

            case 6: // INC
                SP = SP + ir.m;
                strcpy(opStr, "INC");
                break;

            case 7: // JMP
                PC = ir.m;
                strcpy(opStr, "JMP");
                break;

            case 8: // JPC
                if (pas[SP] == 1)
                    PC = ir.m;
                SP--;
                strcpy(opStr, "JPC");
                break;

            case 9: // SYS
                if (ir.m == 1) {
                    printf("%d", pas[SP]);
                    SP--;
                }
                else if (ir.m == 2) {
                    SP++;
                    scanf("%d", &pas[SP]);
                }
                else if (ir.m == 3) {
                    Halt = 0;
                }
                strcpy(opStr, "SYS");
                break;

            default:
                // FIXME
                printf("Error in op switch\n");
                Halt = 0;
                break;
        } // end op switch

        //TODO Print status to file
        printf("%d %s %d %d\t%d\t%d\t%d", initialPC, opStr, ir.l, ir.m, PC, BP, SP);
        printf("\n");

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
