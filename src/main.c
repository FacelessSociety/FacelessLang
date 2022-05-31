#include <stdio.h>
#include <unistd.h>
#include <colors.h>
#include <stdlib.h>
#include <lexer.h>

// 2022 Ian Moffett


static FILE* fp;


void panic(void) {
    fclose(fp);
    exit(1);
}


static void run(const char* filename) {
    fp = fopen(filename, "r");

    lexer_init(fp);

    fclose(fp);
}



int main(int argc, char** argv) {
    for (int i = 0; i < argc; ++i) {
        if (argv[i][0] != '-') {
            run(argv[i]);
        }
    }
}
