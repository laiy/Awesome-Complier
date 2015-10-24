/* 本程序统计并打印文件中数字的数目和ID的数目 */
%{
   #include "stdio.h"
   #include "stdlib.h"

   int num_num = 0, num_id = 0;
%}

INTEGER   [-+]?[1-9][0-9]*
ID        [_a-zA-Z][a-zA-Z_0-9]*
SPACE     [\ \n\t]

%%

{INTEGER} {
            num_num++;                          /*数字数加一*/
            printf("(num = %d)\n", atoi(yytext)); /*打印数字值*/
}

{ID}      {
            num_id++;                           /*ID数加一*/
            printf("(id = %s)\n", yytext);        /*打印ID字符串*/
}

{SPACE}|\. {}

%%

int main()
{
   yylex();
   printf("num=%d, id=%d\n", num_num, num_id);
   return 0;
}

int yywrap() // 此函数必须由用户提供, 函数的返回值是1，就表示停止解析
{
   return 1;
}
