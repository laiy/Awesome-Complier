/*
 * Regular expression implementation.
 * Supports traditional egrep syntax, plus non-greedy operators.
 * Tracks submatches a la traditional backtracking.
 *
 * Normally finds leftmost-biased (traditional backtracking) match;
 * run with -l to get leftmost-longest match (but not POSIX submatches).
 *
 * Normally executes repetitions as much as possible, but no more than
 * necessary -- i.e. no unnecessary repeats that match the empty string --
 * but this differs from Perl.  Run with -p to get exact Perl behavior.
 *
 * Copyright (c) 2007 Russ Cox.
 * Can be distributed under the MIT license, see bottom of file.
 */

#include <iostream>
#include <vector>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

enum
{
    LeftmostBiased = 0,
    LeftmostLongest = 1,
};

enum
{
    RepeatMinimal = 0,
    RepeatLikePerl = 1,
};

int debug;
int matchtype = LeftmostBiased;
int reptype = RepeatMinimal;

enum
{
    NSUB = 10
};

typedef struct Sub Sub;
struct Sub
{
    const char *sp;
    const char *ep;
};

enum {
    CharIncluded = 0,
    CharExcluded = 1,
};

enum {
    NCHAR = 128,
};
typedef struct Range Range;
struct Range {
    int type;
    char flag[NCHAR];
};

typedef union Data Data;
union Data
{
    int val;
    Range range;
};

enum
{
    Char = 1,
    Any = 2,
    Split = 3,
    LParen = 4,
    RParen = 5,
    Match = 6,
    CharClass = 7,
};
typedef struct State State;
typedef struct Thread Thread;
struct State
{
    int op;
    Data data;
    State *out;
    State *out1;
    int id;
    int lastlist;
    int visits;
    Thread *lastthread;
};

struct Thread
{
    State *state;
    Sub match[NSUB];
};

typedef struct List List;
struct List
{
    Thread *t;
    int n;
};

State matchstate = { Match };
int nstate;
int listid;
List l1, l2;

/* Allocate and initialize State */
State*
state(int op, int val, State *out, State *out1)
{
    State *s;

    nstate++;
    s = (State *)malloc(sizeof *s);
    s->lastlist = 0;
    s->op = op;
    s->data.val = val;
    s->out = out;
    s->out1 = out1;
    s->id = nstate;
    return s;
}

/* Allocate and initialize CharClass State */
State*
ccstate(int op, Range range, State *out, State *out1)
{
    State *s;

    nstate++;
    s = (State *)malloc(sizeof *s);
    s->lastlist = 0;
    s->op = op;
    s->data.range = range;
    s->out = out;
    s->out1 = out1;
    s->id = nstate;
    return s;
}

typedef struct Frag Frag;
typedef union Ptrlist Ptrlist;
struct Frag
{
    State *start;
    Ptrlist *out;
};

/* Initialize Frag struct. */
Frag
frag(State *start, Ptrlist *out)
{
    Frag n = { start, out };
    return n;
}

/*
 * Since the out pointers in the list are always
 * uninitialized, we use the pointers themselves
 * as storage for the Ptrlists.
 */
union Ptrlist
{
    Ptrlist *next;
    State *s;
};

/* Create singleton list containing just outp. */
Ptrlist*
list1(State **outp)
{
    Ptrlist *l;

    l = (Ptrlist*)outp;
    l->next = NULL;
    return l;
}

/* Patch the list of states at out to point to start. */
void
patch(Ptrlist *l, State *s)
{
    Ptrlist *next;

    for(; l; l=next){
        next = l->next;
        l->s = s;
    }
}

/* Join the two lists l1 and l2, returning the combination. */
Ptrlist*
append(Ptrlist *l1, Ptrlist *l2)
{
    Ptrlist *oldl1;

    oldl1 = l1;
    while(l1->next)
        l1 = l1->next;
    l1->next = l2;
    return oldl1;
}

int nparen;
void yyerror(const char*);
int yylex(void);
State *start;

Frag
paren(Frag f, int n)
{
    State *s1, *s2;

    if(n >= NSUB)
        return f;
    s1 = state(LParen, n, f.start, NULL);
    s2 = state(RParen, n, NULL, NULL);
    patch(f.out, s2);
    return frag(s1, list1(&s2->out));
}

typedef union YYSTYPE YYSTYPE;
union YYSTYPE {
    Frag frag;
    int c;
    int nparen;
    Range range;
};
YYSTYPE yylval;

const char *input;
const char *text;
void dumplist(List*);

enum
{
    EOL = 0,
    CHAR = 257,
    CHARCLASS = 258,
};

int
yylex(void)
{
    int c;

    if(input == NULL || *input == 0)
        return EOL;
    c = *input++;
    /* escape character */
    if (c == '\\') {
        c = *input++;
        switch (c) {
            case '\0':
                yyerror("invalid regexp");
                exit(1);
            case 'r':
                yylval.c = '\r';
                break;
            case 'n':
                yylval.c = '\n';
                break;
            case 't':
                yylval.c = '\t';
                break;
            default:
                yylval.c = c;
                break;
        }
        return CHAR;
    }
    /* character class */
    if (c == '[') {
        int i, nchar = 0, ndash = 0;
        char lastchar;
        yylval.range.type = CharIncluded;
        if (*input == '^') {
            yylval.range.type = CharExcluded;
            input++;
        }
        if (*input == ']') {
            yyerror("invalid regexp");
            exit(1);
        }
        memset(yylval.range.flag, 0, sizeof(yylval.range.flag));
        while (*input != 0) {
            c = *input++;
            if (c == ']') {
                if (nchar > 0)
                    yylval.range.flag[lastchar] = 1;
                if (ndash > 0)
                    yylval.range.flag['-'] = 1;
                if (yylval.range.type == CharExcluded) {
                    for (i=0; i<NCHAR; i++)
                        yylval.range.flag[i] = 1-yylval.range.flag[i];
                }
                return CHARCLASS;
            }
            if (c == '-') {
                ndash++;
                continue;
            }
            if (c == '\\') {
                c = *input++;
                switch (c) {
                    case '\0':
                        yyerror("invalid regexp");
                        exit(1);
                    case 'r':
                        c = '\r';
                        break;
                    case 'n':
                        c = '\n';
                        break;
                    case 't':
                        c = '\t';
                        break;
                    default:
                        break;
                }
            }
            if (nchar > 0 && ndash > 0) {
                nchar = ndash = 0;
                if (lastchar > c) {
                    yyerror("invalid regexp");
                    exit(1);
                } else {
                    for (i=lastchar; i<=c; i++)
                        yylval.range.flag[i] = 1;
                }
            } else if (nchar > 0) {
                yylval.range.flag[lastchar] = 1;
                lastchar = c;
            } else if (ndash > 0) {
                ndash = 0;
                yylval.range.flag['-'] = 1;
                nchar++;
                lastchar = c;
            } else {
                nchar++;
                lastchar = c;
            }
        }
        yyerror("invalid regexp");
        exit(1);
    }
    if(strchr("|*+?():.", c))
        return c;
    yylval.c = c;
    return CHAR;
}

int look;

void
move()
{
    look = yylex();
}

int
matchtoken(int t)
{
    if (look == t) {
        move();
        return 1;
    }
    return 0;
}

Frag single();
Frag repeat();
Frag concat();
Frag alt();
void line();

void
line()
{
    Frag alt1 = alt();
    if (!matchtoken(EOL))
        yyerror("expected EOL");
    State *s;
    alt1 = paren(alt1, 0);
    s = state(Match, 0, NULL, NULL);
    patch(alt1.out, s);
    start = alt1.start;
}

Frag
alt()
{
    Frag concat1 = concat();
    while (matchtoken('|')) {
        Frag concat2 = concat();
        State *s = state(Split, 0, concat1.start, concat2.start);
        concat1 = frag(s, append(concat1.out, concat2.out));
    }
    return concat1;
}

Frag
concat()
{
    Frag repeat1 = repeat();
    while (look!=EOL && look!='|' && look!=')') {
        Frag repeat2 = repeat();
        patch(repeat1.out, repeat2.start);
        repeat1 = frag(repeat1.start, repeat2.out);
    }
    return repeat1;
}

Frag
repeat()
{
    Frag single1 = single();
    if (matchtoken('*')) {
        if (matchtoken('?')) {
            State *s = state(Split, 0, NULL, single1.start);
            patch(single1.out, s);
            return frag(s, list1(&s->out));
        } else {
            State *s = state(Split, 0, single1.start, NULL);
            patch(single1.out, s);
            return frag(s, list1(&s->out1));
        }
    } else if (matchtoken('+')) {
        if (matchtoken('?')) {
            State *s = state(Split, 0, NULL, single1.start);
            patch(single1.out, s);
            return frag(single1.start, list1(&s->out));
        } else {
            State *s = state(Split, 0, single1.start, NULL);
            patch(single1.out, s);
            return frag(single1.start, list1(&s->out1));
        }
    } else if (matchtoken('?')) {
        if (matchtoken('?')) {
            State *s = state(Split, 0, NULL, single1.start);
            return frag(s, append(single1.out, list1(&s->out)));
        } else {
            State *s = state(Split, 0, single1.start, NULL);
            return frag(s, append(single1.out, list1(&s->out1)));
        }
    }
    return single1;
}

Frag
single()
{
    if (matchtoken('(')) {
        if (matchtoken('?')) {
            if (matchtoken(':')) {
                Frag alt1 = alt();
                matchtoken(')');
                return alt1;
            }
        } else {
            int n = ++nparen;
            Frag alt1 = alt();
            matchtoken(')');
            return paren(alt1, n);
        }
    } else if (matchtoken('.')) {
        State *s = state(Any, 0, NULL, NULL);
        return frag(s, list1(&s->out));
    } else if (look == CHAR) {
        State *s = state(Char, yylval.c, NULL, NULL);
        move();
        return frag(s, list1(&s->out));
    } else if (look == CHARCLASS) {
        State *s = ccstate(CharClass, yylval.range, NULL, NULL);
        move();
        return frag(s, list1(&s->out));
    } else {
        yyerror("single");
    }
}

void
yyparse()
{
    move();
    line();
}

void
yyerror(const char *s)
{
    fprintf(stderr, "parse error: %s\n", s);
    exit(1);
}

void
printmatch(Sub *m, int n)
{
    int i;

    for(i=0; i<n; i++){
        if(m[i].sp && m[i].ep)
            printf("(%d,%d)", (int)(m[i].sp - text), (int)(m[i].ep - text));
        else if(m[i].sp)
            printf("(%d,?)", (int)(m[i].sp - text));
        else
            printf("(?,?)");
    }
}

void
dumplist(List *l)
{
    int i;
    Thread *t;

    for(i=0; i<l->n; i++){
        t = &l->t[i];
        if(t->state->op != Char && t->state->op != CharClass && t->state->op != Any && t->state->op != Match)
            continue;
        printf("  ");
        printf("%d ", t->state->id);
        printmatch(t->match, nparen+1);
        printf("\n");
    }
}

/*
 * Is match a longer than match b?
 * If so, return 1; if not, 0.
 */
int
longer(Sub *a, Sub *b)
{
    if(a[0].sp == NULL)
        return 0;
    if(b[0].sp == NULL || a[0].sp < b[0].sp)
        return 1;
    if(a[0].sp == b[0].sp && a[0].ep > b[0].ep)
        return 1;
    return 0;
}

/*
 * Add s to l, following unlabeled arrows.
 * Next character to read is p.
 */
void
addstate(List *l, State *s, Sub *m, const char *p)
{
    Sub save;

    if(s == NULL)
        return;

    if(s->lastlist == listid){
        switch(matchtype){
            case LeftmostBiased:
                if(reptype == RepeatMinimal || ++s->visits > 2)
                    return;
                break;
            case LeftmostLongest:
                if(!longer(m, s->lastthread->match))
                    return;
                break;
        }
    }else{
        s->lastlist = listid;
        s->lastthread = &l->t[l->n++];
        s->visits = 1;
    }
    if(s->visits == 1){
        s->lastthread->state = s;
        memmove(s->lastthread->match, m, NSUB*sizeof m[0]);
    }

    switch(s->op){
        case Split:
            /* follow unlabeled arrows */
            addstate(l, s->out, m, p);
            addstate(l, s->out1, m, p);
            break;

        case LParen:
            /* record left paren location and keep going */
            save = m[s->data.val];
            m[s->data.val].sp = p;
            m[s->data.val].ep = NULL;
            addstate(l, s->out, m, p);
            /* restore old information before returning. */
            m[s->data.val] = save;
            break;

        case RParen:
            /* record right paren location and keep going */
            save = m[s->data.val];
            m[s->data.val].ep = p;
            addstate(l, s->out, m, p);
            /* restore old information before returning. */
            m[s->data.val] = save;
            break;
    }
}

/*
 * Step the NFA from the states in clist
 * past the character c,
 * to create next NFA state set nlist.
 * Record best match so far in match.
 */
void
step(List *clist, int c, const char *p, List *nlist, Sub *match)
{
    int i;
    Thread *t;
    static Sub m[NSUB];

    if(debug){
        dumplist(clist);
        printf("%c (%d)\n", c, c);
    }

    listid++;
    nlist->n = 0;

    for(i=0; i<clist->n; i++){
        t = &clist->t[i];
        if(matchtype == LeftmostLongest){
            /*
             * stop any threads that are worse than the
             * leftmost longest found so far.  the threads
             * will end up ordered on the list by start point,
             * so if this one is too far right, all the rest are too.
             */
            if(match[0].sp && match[0].sp < t->match[0].sp)
                break;
        }
        switch(t->state->op){
            case Char:
                if(c == t->state->data.val)
                    addstate(nlist, t->state->out, t->match, p);
                break;

            case CharClass:
                if(t->state->data.range.flag[c])
                    addstate(nlist, t->state->out, t->match, p);
                break;

            case Any:
                addstate(nlist, t->state->out, t->match, p);
                break;

            case Match:
                switch(matchtype){
                    case LeftmostBiased:
                        /* best so far ... */
                        memmove(match, t->match, NSUB*sizeof match[0]);
                        /* ... because we cut off the worse ones right now! */
                        return;
                    case LeftmostLongest:
                        if(longer(t->match, match))
                            memmove(match, t->match, NSUB*sizeof match[0]);
                        break;
                }
                break;
        }
    }

    /* start a new thread if no match yet */
    if(match == NULL || match[0].sp == NULL)
        addstate(nlist, start, m, p);
}

/* Compute initial thread list */
List*
startlist(State *start, const char *p, List *l)
{
    List empty = {NULL, 0};
    step(&empty, 0, p, l, NULL);
    return l;
}

int
match(State *start, const char *p, Sub *m)
{
    int c;
    List *clist, *nlist, *t;

    clist = startlist(start, p, &l1);
    nlist = &l2;
    memset(m, 0, NSUB*sizeof m[0]);
    for(; *p && clist->n > 0; p++){
        c = *p & 0xFF;
        step(clist, c, p+1, nlist, m);
        t = clist; clist = nlist; nlist = t;
    }
    step(clist, 0, p, nlist, m);
    return m[0].sp != NULL;
}

void
dump(State *s)
{
    char nc;
    if(s == NULL || s->lastlist == listid)
        return;
    s->lastlist = listid;
    printf("%d| ", s->id);
    switch(s->op){
        case Char:
            printf("'%c' -> %d\n", s->data.val, s->out->id);
            break;

        case CharClass:
            nc = (s->data.range.type == CharExcluded) ? '^' : ' ';
            printf("[%c] -> %d\n", nc, s->out->id);
            break;

        case Any:
            printf(". -> %d\n", s->out->id);
            break;

        case Split:
            printf("| -> %d, %d\n", s->out->id, s->out1->id);
            break;

        case LParen:
            printf("( %d -> %d\n", s->data.val, s->out->id);
            break;

        case RParen:
            printf(") %d -> %d\n", s->data.val, s->out->id);
            break;

        case Match:
            printf("match\n");
            break;

        default:
            printf("??? %d\n", s->op);
            break;
    }

    dump(s->out);
    dump(s->out1);
}

static set<State *> freenodes;
void freenfa(State *state) {
    if (state == NULL)
        return;

    if (freenodes.count(state) == 0) {
        freenodes.insert(state);
        freenfa(state->out);
        freenfa(state->out1);
        free(state);
    }
}

vector<vector<int> >
findall(const char *regex, const char *content) {
    Sub m[NSUB];
    vector<vector<int> > result;

    input = regex;
    nparen = 0;
    yyparse();

    listid = 0;
    if(debug)
        dump(start);

    l1.t = (Thread *)malloc(nstate*sizeof l1.t[0]);
    l2.t = (Thread *)malloc(nstate*sizeof l2.t[0]);

    text = content;	/* used by printmatch */
    const char *pos = content;
    while (*pos) {
        if(match(start, pos, m)){
            if (m[0].ep == m[0].sp) {
                pos++;
                continue;
            }

            vector<int> onematch;
            for (int i=0; i<=nparen; i++) {
                onematch.push_back((int)(m[i].sp-text));
                onematch.push_back((int)(m[i].ep-text));
            }
            result.push_back(onematch);

            pos = m[0].ep;
        } else{
            break;
        }
    }

    free(l1.t);
    free(l2.t);
    freenodes.clear();
    freenfa(start);

    return result;
}

//int main()
//{
//    char regex[] = "([a-z]+)([^a-z]+)";
//    char content[] = "ab2c1def3g12ui";
//    vector<vector<int>> result;
//    result = findall(regex, content);
//    for (int i=0; i<result.size(); i++) {
//        for (int j=result[i][0]; j<result[i][1]; j++)
//            printf("%c", *(content+j));
//        printf(": ");
//        for (int j=0; j<result[0].size(); j+=2)
//            printf("(%d,%d)", result[i][j], result[i][j+1]);
//        printf("\n");
//    }
//    printf("\n");
//}

/*
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

