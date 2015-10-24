%{
    #include <stdio.h>
    #include <string>
    #include <stdlib.h>

    std::string s;
    int length, i, l, temp;
    void output_ip_type(std::string ip);
%}

OTHERS .*
VALID0_255 ((25[0-5])|(2[0-4][0-9]))|((1[0-9]{2})|([1-9]?[0-9]))
IPV4 ({VALID0_255}\.){3}{VALID0_255}

%%

\n{IPV4}\n {
    s = "";
    for (i = 1; i < yyleng - 1; i++)
        s += yytext[i];
    output_ip_type(s);
    yyless(yyleng - 1);
}

\n{OTHERS}\n {
    printf("Invalid\n");
    yyless(yyleng - 1);
}

{IPV4}\n {
    s = "";
    for (i = 0; i < yyleng - 1; i++)
        s += yytext[i];
    output_ip_type(s);
    yyless(yyleng - 1);
}

{OTHERS}\n {
    printf("Invalid\n");
    yyless(yyleng - 1);
}

\n{IPV4} {
    s = "";
    for (i = 1; i < yyleng; i++)
        s += yytext[i];
    output_ip_type(s);
    yyless(yyleng - 1);
}

\n{OTHERS} {
    printf("Invalid\n");
    yyless(yyleng - 1);
}

%%

void output_ip_type(std::string ip) {
    s = "";
    length = ip.length();
    l = 0, i = 0;
    while (ip[i] != '.') i++;
    for (; l < i; l++)
        s += ip[l];
    temp = atoi(s.c_str());
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

