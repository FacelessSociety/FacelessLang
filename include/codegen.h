#ifndef CODEGEN_H
#define CODEGEN_H

// 2022 Ian Moffett

#include <AST.h>

typedef uint8_t REG_COUNTER;

typedef enum {
    U8,
} INTEGER_TYPE;


int32_t interpret_ast(struct ASTNode* node, REG_COUNTER reg);
void codegen_init(void);
void codegen_end(void);
void codegen_printint(REG_COUNTER reg);
void codegen_genglobsym(const char* sym, INTEGER_TYPE type);

#endif
