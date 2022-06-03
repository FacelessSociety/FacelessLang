#include <stdio.h>
#include <unistd.h>
#include <colors.h>
#include <stdlib.h>
#include <lexer.h>
#include <parse.h>
#include <AST.h>
#include <codegen.h>
#include <symbol.h>
#include <string.h>

// 2022 Ian Moffett

#if defined(_WIN32) || defined(WIN32)
#error LINUX ONLY.
#endif


FILE* in;
const char* cur_filename;
uint8_t asm_only = 0;


void panic(void) {
    fclose(in);
    free_ast();
    codegen_end();
    destroy_symbols();
    exit(1);
}


static void run(const char* filename) {
    cur_filename = filename;
    in = fopen(filename, "r");
    lexer_init();                 // Setup lexer.
    parse();                        // Parse.
    free_ast();                     // Free AST.
    destroy_symbols();
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
            break;
        } else if (strcmp(argv[i], "-S") == 0) {
            asm_only = 1;
        }
    }
}
