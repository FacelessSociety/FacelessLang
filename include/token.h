#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

// 2022 Ian Moffett


typedef enum {
    TT_PLUS,
    TT_MINUS,
    TT_SLASH,
    TT_STAR,
    TT_INTLIT
} TOKEN_TYPE;


struct Token {
    union {
        char* str;
        uint64_t val_int;
    };

    TOKEN_TYPE type;
};


#endif
