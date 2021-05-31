/*********************************************************************
 *  Assignment 1: P-Machine (Virtual Machine)                        *
 *  COP 3402                                                         *
 *  Authors: Willow Maddox                                           *
 *           Michael Bernhardt                                       *
 *********************************************************************/

#include <stdio.h>
#include <string.h>

#define MAX_PAS_LENGTH 500

int base(int);

typedef struct instruction_registry {
    int op; // Op code
    int l;  // Legicographical level
    int m;  // Variable determined by Op code
} iReg;

int PC = 0,     // Program Counter
    BP = 0,     // Base Pointer
    SP = 0,     // Stack Pointer
    Halt = 1;   // Halt flag

int pas[MAX_PAS_LENGTH]; // Process Address Space

int main(int argc, char **argv) {
    
    // Make sure file name was entered in command line
    if (argc < 2) {
        printf("Invalid or no input file specified.\n\n");
        return 0;
    }

    FILE* inputFile;
    int stackBase;

    // Initialize stack
    for (int i = 0; i < MAX_PAS_LENGTH; i++)
        pas[i] = 0;
      
    // Declare instruction registry and initialize fields to 0
    iReg ir;
    ir.op = ir.l = ir.m = 0;

    // Read file from command line into text segment
    inputFile = fopen(argv[1], "r");

    while (fscanf(inputFile, "%d", &pas[SP]) != -1) {
        fscanf(inputFile, " %d %d\n", &pas[SP + 1], &pas[SP + 2]);
        SP += 3;
    }
    fclose(inputFile);

    // Set BP to next open index and SP to last index of text segment
    stackBase = SP;
    BP = SP;
    SP--;
    
    // Print results and print initial values
    printf("\n\n\t\tPC      BP      SP      stack\n");
    printf("Initial values: %2d      %2d      %2d\n\n", PC, BP, SP);

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
                        SP = BP - 1;
                        BP = pas[SP + 2];
                        PC = pas[SP + 3];
                        strcpy(opStr, "RTN");
                        break;

                    case 1: // NEG
                        pas[SP] = (-1) * pas[SP];
                        strcpy(opStr, "NEG");
                        break;

                    case 2: // ADD
                        SP--;
                        pas[SP] = pas[SP] + pas[SP + 1];
                        strcpy(opStr, "ADD");
                        break;

                    case 3: // SUB
                        SP--;
                        pas[SP] = pas[SP] - pas[SP + 1];
                        strcpy(opStr, "SUB");
                        break;

                    case 4: // MUL
                        SP--;
                        pas[SP] = pas[SP] * pas[SP + 1];
                        strcpy(opStr, "MUL");
                        break;

                    case 5: // DIV
                        SP--;
                        pas[SP] = pas[SP] / pas[SP + 1];
                        strcpy(opStr, "DIV");
                        break;

                    case 6: // ODD
                        pas[SP] = pas[SP] % 2;
                        strcpy(opStr, "ODD");
                        break;

                    case 7: // MOD
                        SP--;
                        pas[SP] = pas[SP] % pas[SP + 1];
                        strcpy(opStr, "MOD");
                        break;

                    case 8: // EQL
                        SP--;
                        pas[SP] = (pas[SP] == pas[SP + 1]);
                        strcpy(opStr, "EQL");
                        break;

                    case 9: // NEQ
                        SP--;
                        pas[SP] = (pas[SP] != pas[SP + 1]);
                        strcpy(opStr, "NEQ");
                        break;

                    case 10: // LSS
                        SP--;
                        pas[SP] = (pas[SP] < pas[SP + 1]);
                        strcpy(opStr, "LSS");
                        break;

                    case 11: // LEQ
                        SP--;
                        pas[SP] = (pas[SP] <= pas[SP + 1]);
                        strcpy(opStr, "LEQ");
                        break;

                    case 12: // GTR
                        SP--;
                        pas[SP] = (pas[SP] > pas[SP + 1]);
                        strcpy(opStr, "GTR");
                        break;

                    case 13: // GEQ
                        SP--;
                        pas[SP] = (pas[SP] >= pas[SP + 1]);
                        strcpy(opStr, "GEQ");
                        break;

                    default:
                        printf("Error in OPR case switch\n");
                        break;
                } // End OPR switch
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
                    printf("Top of Stack Value: %d\n", pas[SP]);
                    SP--;
                }
                else if (ir.m == 2) {
                    printf("Please Enter an Integer: ");
                    SP++;
                    scanf("%d", &pas[SP]);
                }
                else if (ir.m == 3) {
                    Halt = 0;
                }
                strcpy(opStr, "SYS");
                break;

            // Prints error and exits if wrong input
            default:
                printf("Error in op switch\n");
                return -1;
        } // end op switch

        // Print status to console
        printf("%2d  %s %2d %2d\t%2d\t%2d\t%2d\t", initialPC, opStr, ir.l,
                ir.m, PC, BP, SP);
                
        int maxLs = 0;

        // Find out how many lexicographical levels there are
        while (base(maxLs) > stackBase)
            maxLs++;
        
        // Print the segment of the stack for each level
        for (; maxLs >= 0; maxLs--) {
            int currBP = base(maxLs);
            int currSP = (maxLs == 0) ? SP : base(maxLs - 1) - 1;

            for (int i = currBP; i <= currSP; i++)
                printf("%d ", pas[i]);

            if (currBP < BP)
                printf("| ");
        }

        printf("\n");

    } // end instruction while loop

    printf("\n");

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
