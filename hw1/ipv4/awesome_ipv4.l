%{
    #include <stdio.h>

    int i, base, temp;
    char s[5];
    void output_ip_type();
%}

OTHERS .*
VALID0_255 ((25[0-5])|(2[0-4][0-9]))|((1[0-9]{2})|([1-9]?[0-9]))
IPV4 ({VALID0_255}\.){3}{VALID0_255}

%%

\n{IPV4}\n {
    for (i = 1; i < 5; i++)
        s[i - 1] = yytext[i];
    output_ip_type();
    yyless(yyleng - 1);
}

\n{OTHERS}\n {
    printf("Invalid\n");
    yyless(yyleng - 1);
}

{IPV4}\n {
    for (i = 0; i < yyleng - 1; i++)
        s[i] += yytext[i];
    output_ip_type();
    yyless(yyleng - 1);
}

{OTHERS}\n {
    printf("Invalid\n");
    yyless(yyleng - 1);
}

\n{IPV4} {
    for (i = 1; i < yyleng; i++)
        s[i - 1] += yytext[i];
    output_ip_type();
    yyless(yyleng - 1);
}

\n{OTHERS} {
    printf("Invalid\n");
    yyless(yyleng - 1);
}

%%

void output_ip_type() {
    base = 1, temp = 0;
    for (i = 2; i >= 0; i--)
        temp += (s[i] - '0') * base, base *= 10;
    if (temp <= 127)
        printf("A\n");
    else if (temp >= 128 && temp <= 191)
        printf("B\n");
    else if (temp >= 192 && temp <= 223)
        printf("C\n");
    else if (temp >= 224 && temp <= 239)
        printf("D\n");
    else
        printf("E\n");
}

