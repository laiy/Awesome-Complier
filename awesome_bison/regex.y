%{
#include "regexp.h"

void yyerror(char*);
static int nparen;
%}

%union {
    Regexp *re;
    int c;
    int nparen;
}

%token	<c> CHAR EOL
%type	<re> alt concat repeat single line
%type	<nparen> count

%%

file:
|	file line

line: alt '\n'
	{
		printre($1);
		printf("\n");
		nparen = 0;
	}
|	alt
    {
        printre($1);
        printf("\n");
        nparen = 0;
    }

alt:
	concat
|	alt '|' concat
	{
		$$ = reg(Alt, $1, $3);
	}
;

concat:
	repeat
|	concat repeat
	{
		$$ = reg(Cat, $1, $2);
	}
;

repeat:
	single
|	single '*'
	{
		$$ = reg(Star, $1, NULL);
	}
|	single '*' '?'
	{
		$$ = reg(Star, $1, NULL);
		$$->n = 1;
	}
|	single '+'
	{
		$$ = reg(Plus, $1, NULL);
	}
|	single '+' '?'
	{
		$$ = reg(Plus, $1, NULL);
		$$->n = 1;
	}
|	single '?'
	{
		$$ = reg(Quest, $1, NULL);
	}
|	single '?' '?'
	{
		$$ = reg(Quest, $1, NULL);
		$$->n = 1;
	}
;

count:
	{
		$$ = ++nparen;
	}
;

single:
	'(' count alt ')'
	{
		$$ = reg(Paren, $3, NULL);
		$$->n = $2;
	}
|	'(' '?' ':' alt ')'
	{
		$$ = $4;
	}
|	CHAR
	{
		$$ = reg(Lit, NULL, NULL);
		$$->ch = $1;
	}
|	'.'
	{
		$$ = reg(Dot, NULL, NULL);
	}
;

%%

void fatal(char *fmt, ...) {
	va_list arg;

	va_start(arg, fmt);
	fprintf(stderr, "fatal error: ");
	vfprintf(stderr, fmt, arg);
	fprintf(stderr, "\n");
	va_end(arg);
	exit(2);
}

void yyerror(char *s) {
	fatal("%s", s);
}

void* mal(int n) {
	void *v;

	v = malloc(n);
	if(v == NULL)
		fatal("out of memory");
	memset(v, 0, n);
	return v;
}

Regexp* reg(int type, Regexp *left, Regexp *right) {
	Regexp *r;

	r = mal(sizeof *r);
	r->type = type;
	r->left = left;
	r->right = right;
	return r;
}

void printre(Regexp *r) {
	switch(r->type) {
        default:
            printf("???");
            break;

        case Alt:
            printf("Alt(");
            printre(r->left);
            printf(", ");
            printre(r->right);
            printf(")");
            break;

        case Cat:
            printf("Cat(");
            printre(r->left);
            printf(", ");
            printre(r->right);
            printf(")");
            break;

        case Lit:
            printf("Lit(%c)", r->ch);
            break;

        case Dot:
            printf("Dot");
            break;

        case Paren:
            printf("Paren(%d, ", r->n);
            printre(r->left);
            printf(")");
            break;

        case Star:
            if(r->n)
                printf("Ng");
            printf("Star(");
            printre(r->left);
            printf(")");
            break;

        case Plus:
            if(r->n)
                printf("Ng");
            printf("Plus(");
            printre(r->left);
            printf(")");
            break;

        case Quest:
            if(r->n)
                printf("Ng");
            printf("Quest(");
            printre(r->left);
            printf(")");
            break;
	}
}

int main(int argc, char **argv) {
	yyparse();
	return 0;
}

