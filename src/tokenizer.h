//
// Created by pwpio on 5/28/2022.
//

#ifndef PAINLANG_TOKENIZER_H
#define PAINLANG_TOKENIZER_H

typedef enum {
    TOK_UNDEFINED,
    TOK_ID,
    TOK_NUM,
    TOK_AT,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_DOT,
    TOK_COMMA,
    TOK_QUESTION_MARK,
    TOK_EXCLAMATION_MARK,
    TOK_COLON,
    TOK_SEMI,
    TOK_DASH,
    TOK_PLUS,
    TOK_SLASH,
    TOK_ASTERISK,
    TOK_DOLLAR
} TokenType;

char *TokenType_ToString(TokenType);
TokenType TokenType_FromChar(char);

typedef struct {
    char c;
    unsigned int line;
    TokenType type;
} Token;

Token *Token_CreateToken(char, unsigned int, TokenType);
void Token_DestroyToken(Token *);

typedef struct {
    char *str;
    unsigned int pos;
    unsigned int line;
} Tokenizer;

Tokenizer *Tokenizer_CreateTokenizer(char *);
void Tokenizer_DestroyTokenizer(Tokenizer *);

Token *Tokenizer_Next(Tokenizer *);

void Token_PrintDebug(Token *);

#endif //PAINLANG_TOKENIZER_H
