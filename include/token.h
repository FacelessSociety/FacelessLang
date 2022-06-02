#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

// 2022 Ian Moffett


typedef enum {
    TT_PLUS,
    TT_MINUS,
    TT_STAR,
    TT_SLASH,
    TT_INTLIT,
    TT_SEMI,
    TT_CONOUT,
    TT_LPAREN,
    TT_RPAREN,
    TT_INVALID
} TOKEN_TYPE;


struct Token {
    union {
        char* str;
        int64_t val_int;
    };

    TOKEN_TYPE type;
};


#endif
