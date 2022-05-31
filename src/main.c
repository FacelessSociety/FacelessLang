#include <stdio.h>
#include <unistd.h>
#include <colors.h>
#include <stdlib.h>
#include <lexer.h>
#include <parse.h>
#include <AST.h>

// 2022 Ian Moffett


FILE* in;


void panic(void) {
    fclose(in);
    free_ast();
    exit(1);
}


static void run(const char* filename) {
    in = fopen(filename, "r");
    lexer_init();                 // Setup lexer.
    parse();                        // Parse.
    free_ast();                     // Free AST.
    fclose(in);
}



int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            if (access(argv[i], F_OK) != 0) {
                printf("fcomp: Could not open \"%s\"!\n", argv[i]);
                exit(1);
            }

            run(argv[i]);
        }
    }
}
