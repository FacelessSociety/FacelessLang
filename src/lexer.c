#include <lexer.h>
#include <ctype.h>
#include <panic.h>
#include <colors.h>
#include <string.h>

#define IDENT_MAX_LENGTH 35


extern FILE* in;
static size_t line = 1;
static uint8_t eof = 0;
static size_t fp_off = 0;
static char last_ident[IDENT_MAX_LENGTH];

void lexer_init(void) {
    if (in != NULL) return;
    eof = 0;
    fp_off = 0;
    line = 1;
}


size_t get_line(void) {
    return line;
}

static char next(void) {
    if (eof) return EOF;

    char c = fgetc(in);

    ++fp_off;
    return c;
}


static void putback(void) {
    fseek(in, fp_off -= 1, SEEK_SET);
}


// Give position of character in string.
static int chrpos(char* s, char c) {
    char* p = strchr(s, c);
    return p ? p - s : -1;
}


// Read in a whole integer literal.
static int scan_int(char c) {
    int k, val = 0;

    while ((k = chrpos("0123456789", c)) >= 0) {
        val = val * 10 + k;
        c = next();
    }

    putback();               // Non-integer character reached, put it back.
    return val;
}


static char skip(void) {
    char c = next();

    while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f') {
        if (c == '\n') ++line;
        c = next();
    }

    return c;
}


uint8_t is_eof(void) {
    return eof;
}


static void scan_ident(char c) {
    uint16_t i = 0;

    while (isalpha(c) || isdigit(c) || c == '_') {
        if (i == IDENT_MAX_LENGTH - 1) {
            printf(COLOR_ERROR "Identifier too long on line %ld\n", line);
            panic();
        }

        last_ident[i++] = c;
        c = next();
    }

    putback();
}


static TOKEN_TYPE check_keyword(void) {
    if (strcmp(last_ident, "conout") == 0) {
        return TT_CONOUT;
    }

    return TT_INVALID;
}


uint8_t scan(struct Token* token) {
    if (eof) return 0;

    char c = skip();

    switch (c) {
        case ';':
            token->type = TT_SEMI;
            break;
        case EOF:
            eof = 1;
            return 0;
        case '+':
            token->type = TT_PLUS;
            break;
        case '-':
            token->type = TT_MINUS;
            break;
        case '*':
            token->type = TT_STAR;
            break;
        case '/':
            token->type = TT_SLASH;
            break; 
        case '(':
            token->type = TT_LPAREN;
            break;
        case ')':
            token->type = TT_RPAREN;
            break;
        default:
            if (isdigit(c)) {
                token->val_int = scan_int(c);
                token->type = TT_INTLIT;
                break;
            }

            scan_ident(c);
            token->type = check_keyword();

            if (token->type != TT_INVALID) break;

            printf(COLOR_ERROR "Unknown token found while scanning.\n");
            panic();
    }

    return 1;
}
