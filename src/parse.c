#include <parse.h>
#include <lexer.h>
#include <panic.h>
#include <expr.h>
#include <AST.h>
#include <token.h>
#include <stdio.h>
#include <stdint.h>


// 2022 Ian Moffett

struct Token cur_token;


// Parse a primary factor.
static struct ASTNode* primary(void) {
    struct ASTNode* n;

    switch (cur_token.type) {
        case TT_INTLIT:
            n = mkastleaf(A_INTLIT, cur_token.val_int);
            scan(&cur_token);
            return n;
        default:
            printf("Syntax error on line %ld\n", get_line());
            panic();
    }

    return NULL;
}


static struct ASTNode* binexpr(void) {
    struct ASTNode *n, *left, *right;
    AST_NODE_TYPE type;

    // Get integer on left.
    left = primary();

    // If there is no tokens left, return the 
    // left node.
    if (is_eof()) {
        return left;
    }

    // Convert token into AST_NODE_TYPE.
    type = arithop(cur_token.type);

    // Read in next token.
    scan(&cur_token);

    // Get right-hand tree.
    right = binexpr();

    // Build and return tree.
    n = mkastnode(type, left, NULL, right, 0);
    return n;
}


static int64_t interpret_ast(struct ASTNode* node) { 
    int64_t leftval, rightval;

    // Get left and right sub-tree vaues.
    if (node->left)
        leftval = interpret_ast(node->left);

    if (node->right)
        rightval = interpret_ast(node->right);

    switch (node->op) {
        case A_ADD:
            return leftval + rightval;
        case A_SUB:
            return leftval - rightval;
        case A_MUL:
            return leftval * rightval;
        case A_DIV:
            return leftval / rightval;
        case A_INTLIT:
            return node->val_int;
        default:
            printf("Unknown AST operator caught in %s().\n", __func__);
            panic();
    }

    return -1;
}


void parse(void) {
    scan(&cur_token);
    printf("%ld\n", interpret_ast(binexpr()));
}
