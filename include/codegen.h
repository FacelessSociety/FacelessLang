#ifndef CODEGEN_H
#define CODEGEN_H

// 2022 Ian Moffett

#include <AST.h>

typedef uint8_t REG_COUNTER;


int32_t interpret_ast(struct ASTNode* node);
void codegen_init(void);
void codegen_end(void);
void codegen_printint(REG_COUNTER reg);

#endif
