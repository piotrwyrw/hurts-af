//
// Created by pwpio on 5/28/2022.
//

#include "util.h"

#include <stdio.h>
#include <stdlib.h>

void printtimes(char *s, unsigned c) {
    for (unsigned i = 0; i < c; i ++) {
        printf("%s", s);
    }
}

void printspacer(unsigned dep) {
    printtimes("  ", dep);
}

char *readf(FILE *f) {
    unsigned len;

    // Get the file size (in bytes)
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    rewind(f);

    // Malloc a string of the required size
    char *s =calloc(len + 1, sizeof(char));

    unsigned i = 0;
    while (!feof(f)) {
        char c = fgetc(f);
        if (feof(f)) {
            break;
        }
        s[i ++] = c;
    }
    s[i] = '\0';

    return s;
}
