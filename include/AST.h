#ifndef AST_H
#define AST_H

// 2022 Ian Moffett

#include <stdint.h>
#include <colors.h>


typedef enum {
    A_ADD,
    A_SUB,
    A_MUL,
    A_DIV,
    A_INTLIT,
    A_LVIDENT,
    A_ASSIGN,
    A_IDENT,
} AST_NODE_TYPE;


struct ASTNode {
    AST_NODE_TYPE op;
    struct ASTNode* right;
    struct ASTNode* mid;
    struct ASTNode* left;

    union {
        int64_t val_int;
        int64_t id;
    };
};
 

struct ASTNode* mkastnode(AST_NODE_TYPE op, struct ASTNode* left, struct ASTNode* mid, struct ASTNode* right, int64_t val_int);
struct ASTNode* mkastleaf(AST_NODE_TYPE op, int val_int);
struct ASTNode* mkastunary(AST_NODE_TYPE op, struct ASTNode* left, int val_int);
void free_ast(void);
#endif
