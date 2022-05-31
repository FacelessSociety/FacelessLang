#ifndef LEXER_H
#define LEXER_H

// 2022 Ian Moffett

#include <stdio.h>
#include <token.h>
#include <stdint.h>

void lexer_init(FILE* fp);
uint8_t scan(struct Token* token);

#endif
