%{
#include "regexp.h"
#include "regex.tab.h"
%}

%%

"|"	return '|';
"*"	return '*';
"+"	return '+';
"?"	return '?';
"("	return '(';
")"	return ')';
":"	return ':';
"."	return '.';
\r?\n	return '\n';
.	{
	yylval.c = yytext[0];
	return CHAR;
}

%%

