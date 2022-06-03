#include <symbol.h>
#include <string.h>
#include <colors.h>
#include <panic.h>
#include <stdio.h>
#include <stdlib.h>


size_t globs = 0;           // Position of next free global variable.
struct SymbolTable gsym[SYM_TBL_MAX_SYMS];

ssize_t find_glob(char* name) {
    for (size_t i = 0; i < globs; ++i) {
        if (strcmp(name, gsym[i].name) == 0) {
            return i;
        }
    }

    return -1;
}


static size_t newglob(void) {
    size_t p = globs++;

    if (p >= SYM_TBL_MAX_SYMS) {
        printf(COLOR_ERROR "Too many global symbols!\n");
        panic();
    }

    return p;
}


size_t add_glob(char* name) {
    int y = find_glob(name);

    // Return slot if it already exists.
    if (y != -1) {
        return y;
    }

    y = newglob();
    gsym[y].name = strdup(name);
    return y;
}


void destroy_symbols(void) {
    for (int i = 0; i < globs; ++i) {
        free(gsym[i].name);
    }
}
