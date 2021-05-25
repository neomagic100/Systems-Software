#include <stdio.h>
#include <stdlib.h>
#define MAX_PAS_LENGTH 500;
#define MAX_INSTRUCTIONS 24;

typedef struct ir {
    int OP;
    int L;
    int M;
} IR;

int PC = 0,
    BC = 0,
    SP = 0,
    Halt = 0;

int pas[MAX_PAS_LENGTH];

int main(int argc, char **argv) {
    // Get input file from command line
    FILE* inputFile;
    inputFile = fopen(argv[1], "r");

    // Initialize stack
    for (int i = 0; i < MAX_PAS_LENGTH; i++)
        pas[i] = 0;




    // FETCH


    // EXECUTE

    return 0;
}
