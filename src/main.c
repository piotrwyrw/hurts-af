#include "tokenizer.h"
#include "parser.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    FILE *f = fopen("test.pain", "r");
    if (!f) {
        printf("File does not exist.\n");
        return 0;
    }
    char *ss = readf(f);

    printf("%s\n", ss);

    fclose(f);

    Tokenizer *t = Tokenizer_CreateTokenizer(ss);

    Parser *p = Parser_CreateParser(t);

    Node *n = Parser_ParseAll(p);
    Node_PrintRecursive(n, 0);
    Node_DestroyRecursive(n);

    Parser_DestroyParser(p);

    free(ss);

    return 0;
}
