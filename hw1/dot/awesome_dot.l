%{
    #include <stdio.h>
%}

KEYWORD ((i)STRICT)|((i)GRAPH)|((i)DIGRAPH)|((i)SUBGRAPH)|((i)NODE)|((i)EDGE)
ID [a-zA-Z\_][a-zA-Z\_0-9]*
STRING \".*\"
NUMBER [-]?((\.[0-9]+)|([0-9]+(\.[0-9]*)?))
SEPARATOR \;|\,|\{|\}|\[|\]
OTHERS --|->|=
WHITESPACE (\ )
COMMENTS (\/\/.*)|(\/\*(.|\n)*\*\/)|(#.*)

%%

{KEYWORD} {
    printf("%s\n", yytext);
}

{ID} {
    printf("%s\n", yytext);
}

{STRING} {
    printf("%s\n", yytext);
}

{NUMBER} {
    printf("%s\n", yytext);
}

{SEPARATOR} {
    printf("%s\n", yytext);
}

{OTHERS} {
    printf("%s\n", yytext);
}

{WHITESPACE}|{COMMENTS}|\n|\t {}

%%

