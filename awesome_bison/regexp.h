#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef struct Regexp Regexp;
struct Regexp {
    int type;
    int n;
    int ch;
    Regexp *left;
    Regexp *right;
};

enum {
    Alt = 1,
    Cat,
    Lit,
    Dot,
    Paren,
    Quest,
    Star,
    Plus,
};

Regexp *reg(int type, Regexp *left, Regexp *right);
void printre(Regexp*);
void fatal(char*, ...);
void *mal(int);

