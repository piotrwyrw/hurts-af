//
// Created by pwpio on 5/28/2022.
//

#include "tokenizer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _(t) case t: return #t;

char *TokenType_ToString(TokenType type) {
    switch (type) {
        _(TOK_ID)
        _(TOK_NUM)
        _(TOK_AT)
        _(TOK_LPAREN)
        _(TOK_RPAREN)
        _(TOK_LBRACE)
        _(TOK_RBRACE)
        _(TOK_LBRACKET)
        _(TOK_RBRACKET)
        _(TOK_DOT)
        _(TOK_COMMA)
        _(TOK_QUESTION_MARK)
        _(TOK_EXCLAMATION_MARK)
        _(TOK_COLON)
        _(TOK_SEMI)
        _(TOK_DASH)
        _(TOK_PLUS)
        _(TOK_SLASH)
        _(TOK_ASTERISK)
        _(TOK_DOLLAR)
        default:
            return "(unknown)";
    }
}

#undef _

#define _(c, _c) case c: return _c;

TokenType TokenType_FromChar(char c) {
    switch (c) {
        _('@', TOK_AT)
        _('(', TOK_LPAREN)
        _(')', TOK_RPAREN)
        _('{', TOK_LBRACE)
        _('}', TOK_RBRACE)
        _('[', TOK_LBRACKET)
        _(']', TOK_RBRACKET)
        _('.', TOK_DOT)
        _(',', TOK_COMMA)
        _('?', TOK_QUESTION_MARK)
        _('!', TOK_EXCLAMATION_MARK)
        _(':', TOK_COLON)
        _(';', TOK_SEMI)
        _('-', TOK_DASH)
        _('+', TOK_PLUS)
        _('/', TOK_SLASH)
        _('*', TOK_ASTERISK)
        _('$', TOK_DOLLAR)
        default:
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
                return TOK_ID;
            if (c >= '0' && c <= '9')
                return TOK_NUM;
            return TOK_UNDEFINED;
    }
}

#undef _

Token *Token_CreateToken(char c, unsigned int line, TokenType type) {
    Token *tok = malloc(sizeof(Token));
    tok->c = c;
    tok->line = line;
    tok->type = type;
    return tok;
}

void Token_DestroyToken(Token *tok) {
    free(tok);
}

Tokenizer *Tokenizer_CreateTokenizer(char *s) {
    Tokenizer *t = malloc(sizeof(Tokenizer));
    t->str = calloc(strlen(s) + 1, sizeof(char));
    strcpy(t->str, s);
    t->pos = 0;
    t->line = 1;
    return t;
}

void Tokenizer_DestroyTokenizer(Tokenizer *tokenizer) {
    free(tokenizer->str);
    free(tokenizer);
}

Token *Tokenizer_Next(Tokenizer *tokenizer) {
    if (tokenizer->pos >= strlen(tokenizer->str)) {
        return NULL;
    }
    for (;;) {
        char c = tokenizer->str[tokenizer->pos];
        if (c == ' ' || c == '\t' || c == '\n') {
            if (c == '\n') {
                tokenizer->line ++;
            }
            tokenizer->pos ++;
            continue;
        }

        Token *t = Token_CreateToken(c, tokenizer->line, TokenType_FromChar(c));
        tokenizer->pos ++;
        return t;
    }
    return NULL;
}

void Token_PrintDebug(Token *token) {
    printf("(\'%c\' :: %s @ %d)\n", token->c, TokenType_ToString(token->type), token->line);
}