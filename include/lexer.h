#ifndef LEXER_H
#define LEXER_H

// 2022 Ian Moffett

#include <stdio.h>
#include <token.h>
#include <stdint.h>
#include <stddef.h>

void lexer_init(void);
uint8_t scan(struct Token* token);
uint8_t is_eof(void);
size_t get_line(void);
const char* lexer_get_last_ident(void);

#endif
