#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

// 2022 Ian Moffett


typedef enum {
    TT_PLUS,
    TT_MINUS,
    TT_STAR,
    TT_SLASH,
    TT_EQ,
    TT_NE,
    TT_LT,
    TT_GT,
    TT_LE,
    TT_GE,
    TT_INTLIT,
    TT_SEMI,
    TT_CONOUT,
    TT_LPAREN,
    TT_RPAREN,
    TT_U8,
    TT_IDENT,
    TT_ASSIGN,
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
