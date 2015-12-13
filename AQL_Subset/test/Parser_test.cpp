#include "../Parser.h"
#include <iostream>

int main() {
    char perloc_aql_file_path[] = "../dataset/PerLoc.aql";
    char revenue_aql_file_path[] = "../dataset/Revenue.aql";
    Lexer perloc_lexer = Lexer(perloc_aql_file_path);
    Lexer revenue_lexer = Lexer(revenue_aql_file_path);
    char Perloc[] = "../dataset/perloc/PerLoc.input";
    char Revenue[] = "../dataset/revenue/Revenue.input";
    Tokenizer tester1(Perloc);
    Tokenizer tester2(Revenue);
    Parser parser1(perloc_lexer, tester1);
    Parser parser2(revenue_lexer, tester2);
    std::cout << "======================Perloc======================" << std::endl;
    parser1.program();
    std::cout << "======================Revenue=====================" << std::endl;
    parser2.program();
    return 0;
}

