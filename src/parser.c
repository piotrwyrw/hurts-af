//
// Created by pwpio on 5/28/2022.
//

#include "parser.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>

#define _(c, t) case c: return t;

BinaryOp BinaryOp_FromToken(Token *token) {
    switch (token->c) {
        _('+', OP_ADD)
        _('-', OP_SUB)
        _('*', OP_MUL)
        _('/', OP_DIV)
    }
}

#undef _

#define _(t) case t: return #t;

char *BinaryOp_ToString(BinaryOp op) {
    switch (op) {
        _(OP_ADD)
        _(OP_SUB)
        _(OP_MUL)
        _(OP_DIV)
    }
}

#undef _

Node *Node_CreateBase(NodeType type) {
    Node *n = malloc(sizeof(Node));
    n->type = type;
    return n;
}

void Node_DestroyRecursive(Node *node) {
    switch (node->type) {
        case NODE_LITERAL:
            Node_DestroyLiteral(node);
            break;
        case NODE_BINARY_EXPR:
            Node_DestroyBinary(node);
            break;
        case NODE_VAR_DECL:
            Node_DestroyVariableDeclaration(node);
            break;
    }
}

Node *Node_CreateCopy(Node *node) {
    switch (node->type) {
        case NODE_LITERAL:
            return Node_CreateLiteral(node->v.literal.n);
        case NODE_BINARY_EXPR:
            return Node_CreateBinary(node->v.binary.left, node->v.binary.right, node->v.binary.op);
    }
}

#define PRINT(...) \
        printspacer(dep); \
        printf(__VA_ARGS__)

void Node_PrintRecursive(Node *n, unsigned dep) {
    switch (n->type) {
        case NODE_LITERAL:
            PRINT("Integer Literal -> %d\n", n->v.literal.n);
            break;
        case NODE_BINARY_EXPR:
            PRINT("Binary Expression :: %s ->\n", BinaryOp_ToString(n->v.binary.op));
            dep ++;
            PRINT("Left ->\n");
            dep ++;
            Node_PrintRecursive(n->v.binary.left, dep);
            dep --;
            PRINT("Right ->\n");
            dep ++;
            Node_PrintRecursive(n->v.binary.right, dep);
            dep -= 2;
            break;
        case NODE_VAR_DECL:
            PRINT("Variable declaration ->\n");
            dep ++;
            PRINT("Identifier -> %c\n", n->v.variable_declaration.id->c);
            PRINT("Expression ->\n");
            dep ++;
            Node_PrintRecursive(n->v.variable_declaration.expr, dep);
            dep --;
            break;
        default:
            PRINT("(Unknown node type '%d')\n", n->type);
    }
}

#undef PRINT

Node *Node_CreateLiteral(int n) {
    Node *node = Node_CreateBase(NODE_LITERAL);
    node->v.literal.n = n;
    return node;
}

void Node_DestroyLiteral(Node *node) {
    free(node);
}

Node *Node_CreateBinary(Node *left, Node *right, BinaryOp op) {
    Node *n = Node_CreateBase(NODE_BINARY_EXPR);
    n->v.binary.left = left;
    n->v.binary.right = right;
    n->v.binary.op = op;
}

void Node_DestroyBinary(Node *node) {
    Node_DestroyRecursive(node->v.binary.left);
    Node_DestroyRecursive(node->v.binary.right);
    free(node);
}

Node *Node_CreateVariableDeclaration(Token *id, Node *expr) {
    Node *n = Node_CreateBase(NODE_VAR_DECL);
    n->v.variable_declaration.id = id;
    n->v.variable_declaration.expr = expr;
    return n;
}

void Node_DestroyVariableDeclaration(Node *n) {
    Token_DestroyToken(n->v.variable_declaration.id);
    Node_DestroyRecursive(n->v.variable_declaration.expr);
    free(n);
}

Parser *Parser_CreateParser(Tokenizer *tokenizer) {
    Parser *p = malloc(sizeof(Parser));
    p->tokenizer = tokenizer;
    p->current = NULL;
    p->next = NULL;
    Parser_Consume(p);
    return p;
}

void Parser_DestroyParser(Parser *parser) {
    Tokenizer_DestroyTokenizer(parser->tokenizer);
    if (parser->current != NULL)
        Token_DestroyToken(parser->current);
    if (parser->next != NULL)
        Token_DestroyToken(parser->next);
    free(parser);
}

void Parser_Consume(Parser *parser) {
    if (parser->current == NULL && parser->next == NULL) {
        parser->current = Tokenizer_Next(parser->tokenizer);
        parser->next = Tokenizer_Next(parser->tokenizer);
        return;
    }
    if (parser->current != NULL)
        Token_DestroyToken(parser->current);
    parser->current = parser->next;
    parser->next = Tokenizer_Next(parser->tokenizer);
}

// ~~~~~~~~ Parser Functions ~~~~~~~~

#define PARSE_ERROR(tok, ...) \
        printf("(Syntax Error at line %d \'%c\' :: %s) ", tok->line, tok->c, TokenType_ToString(tok->type)); \
        printf(__VA_ARGS__);  \
        exit(0);

Node *Parser_ParseLiteral(Parser *parser) {
    // Current token: Integer literal (?)
    if (parser->current->type != TOK_NUM) {
        PARSE_ERROR(parser->current, "Expected number.\n");
    }

    char s[] = {parser->current->c, 0};
    int v = atoi(s);
    return Node_CreateLiteral(v);
}

Node *Parser_ParseFactor(Parser *parser) {
    if (parser->current->type == TOK_NUM) {
        Node *l = Parser_ParseLiteral(parser);
        Parser_Consume(parser);
        return l;
    } else if (parser->current->type == TOK_LPAREN) {
        Parser_Consume(parser);
        Node *n = Parser_ParseAdditiveExpression(parser);
        if (parser->current->type != TOK_RPAREN) {
            PARSE_ERROR(parser->current, "Expected right parentheses \')\'\n");
        }
        Parser_Consume(parser);
        return n;
    }
}

Node *Parser_ParseAdditiveExpression(Parser *parser) {
    Node *left = Parser_ParseMultiplicativeExpression(parser);
    if (parser->current == NULL) return left;
    if (parser->current->type == TOK_PLUS || parser->current->type == TOK_DASH) {
        BinaryOp op = BinaryOp_FromToken(parser->current);
        Parser_Consume(parser);
        Node *right =
                Node_CreateBinary(left, Parser_ParseAdditiveExpression(parser), op);
        left = right;
    }
    return left;
}

Node *Parser_ParseMultiplicativeExpression(Parser *parser) {
    Node *left = Parser_ParseFactor(parser);
    if (parser->current == NULL) return left;
    if (parser->current->type == TOK_ASTERISK || parser->current->type == TOK_SLASH) {
        BinaryOp op = BinaryOp_FromToken(parser->current);
        Parser_Consume(parser);
        Node *right =
                Node_CreateBinary(left, Parser_ParseMultiplicativeExpression(parser), op);
        left = right;
    }
    return left;
}

Node *Parser_ParseVariableDeclaration(Parser *parser) {
    Token *id;
    Node *expr;
    if (parser->current->type != TOK_DOLLAR) {
        PARSE_ERROR(parser->current, "Expected dollar sign '$' before variable name");
    }
    Parser_Consume(parser);
    if (parser->current->type != TOK_ID) {
        PARSE_ERROR(parser->current, "Expected identifier after '$'");
    }
    id = Token_CreateToken(parser->current->c,  parser->current->line, parser->current->line);
    Parser_Consume(parser);
    if (parser->current->type != TOK_COLON) {
        PARSE_ERROR(parser->current, "Expected ':' after identifier.");
    }
    Parser_Consume(parser);
    expr = Parser_ParseAdditiveExpression(parser);
    if (parser->current->type != TOK_SEMI) {
        PARSE_ERROR(parser->current, "Expected ';' after expression/");
    }
    return Node_CreateVariableDeclaration(id, expr);
}


#undef PARSE_ERRO