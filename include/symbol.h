#ifndef SYMBOL_H
#define SYMBOL_H

// 2022 Ian Moffett

#include <stddef.h>
#include <sys/types.h>

#define SYM_TBL_MAX_SYMS 1024



struct SymbolTable {
    char* name;
};


ssize_t find_glob(char* name);
size_t add_glob(char* name);
void destroy_symbols(void);

extern struct SymbolTable gsym[SYM_TBL_MAX_SYMS];

#endif
