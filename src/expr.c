#include <expr.h>
#include <panic.h>
#include <colors.h>
#include <stdio.h>

// 2022 Ian Moffett.



AST_NODE_TYPE arithop(TOKEN_TYPE token) {
    switch (token) {
        case TT_PLUS:
            return A_ADD;
        case TT_MINUS:
            return A_SUB;
        case TT_STAR:
            return A_MUL;
        case TT_SLASH:
            return A_DIV;
        default: 
            printf("Invalid token caught in %s()!", __func__);
            panic();
    }

    return -1;
}
