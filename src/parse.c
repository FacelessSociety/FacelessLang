#include <parse.h>
#include <lexer.h>
#include <panic.h>
#include <expr.h>
#include <AST.h>
#include <codegen.h>
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

// Ensure the next token is @tok.
static void match(TOKEN_TYPE tok, char* what) {
    if (cur_token.type != tok) {
        printf(COLOR_ERROR "%s expected on line %ld\n", what, get_line());
        printf(COLOR_YELLOW "<tok@%d>\n", cur_token.type);
        panic();
    }
}


// Ensure the next token is a semicolon.
static void semi(void) {
    match(TT_SEMI, "';'");
    scan(&cur_token);
}


// Ensure next token is lparen.
static void lparen(void) {
    match(TT_LPAREN, "'('");
    scan(&cur_token);
}

// Ensure next token is rparen.
static void rparen(void) {
    match(TT_RPAREN, "')'");
    scan(&cur_token);
}


static struct ASTNode* binexpr(void) {
    struct ASTNode *n, *left, *right;
    AST_NODE_TYPE type;

    // Get integer on left.
    left = primary();

    // If there is no tokens left, return the 
    // left node.
    if (cur_token.type == TT_SEMI || cur_token.type == TT_RPAREN) {
        return left;
    } else if (is_eof()) {
        semi();                 // This won't be called, it will fail.
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


static void conout(void) {
    scan(&cur_token);
    lparen();
    struct ASTNode* tree = binexpr();
    rparen();
    semi();

    codegen_printint(interpret_ast(tree));
}



static void keyword(void) {
    switch (cur_token.type) {
        case TT_CONOUT:
            conout();
            break;
        default: break;
    }
}


void parse(void) {
    codegen_init();
    scan(&cur_token);
    keyword();
    codegen_end();
}
