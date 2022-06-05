//
// Created by pwpio on 5/28/2022.
//

#ifndef PAINLANG_PARSER_H
#define PAINLANG_PARSER_H

#include "tokenizer.h"

typedef enum {
    NODE_PROGRAM,
    NODE_LITERAL,
    NODE_BINARY_EXPR,
    NODE_VAR_DECL,
} NodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
} BinaryOp;

BinaryOp BinaryOp_FromToken(Token *);
char *BinaryOp_ToString(BinaryOp);

typedef struct Node Node;

struct Node {
    NodeType type;
    union {
        struct {
            unsigned len;
            Node **nodes;
        } program;
        struct { int n; } literal;
        struct {
            Node *left;
            Node *right;
            BinaryOp op;
        } binary;
        struct {
            Token *id;
            Node *expr;
        } variable_declaration;
    } v;
};

Node *Node_CreateBase(NodeType);
void Node_DestroyRecursive(Node *);
Node *Node_CreateCopy(Node *);

void Node_PrintRecursive(Node *, unsigned);

Node *Node_CreateProgram(Node **, unsigned);
void Node_DestroyProgram(Node *);

Node *Node_CreateLiteral(int);
void Node_DestroyLiteral(Node *);

Node *Node_CreateBinary(Node *, Node *, BinaryOp);
void Node_DestroyBinary(Node *);

Node *Node_CreateVariableDeclaration(Token *, Node *);
void Node_DestroyVariableDeclaration(Node *);

typedef struct {
    Tokenizer *tokenizer;
    Token *current;
    Token *next;
} Parser;

Parser *Parser_CreateParser(Tokenizer *);
void Parser_DestroyParser(Parser *);
void Parser_Consume(Parser *);

// ~~~~~~~~ Parser Functions ~~~~~~~~

Node *Parser_ParseAll(Parser *);
Node *Parser_ParseAny(Parser *);

Node *Parser_ParseLiteral(Parser *);
Node *Parser_ParseAdditiveExpression(Parser *);
Node *Parser_ParseMultiplicativeExpression(Parser *);
Node *Parser_ParseFactor(Parser *);
Node *Parser_ParseVariableDeclaration(Parser *);

#endif //PAINLANG_PARSER_H
