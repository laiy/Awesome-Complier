#include "../Parser.h"
#include <iostream>

int main() {
    char perloc_aql_file_path[] = "../dataset/PerLoc.aql";
    char revenue_aql_file_path[] = "../dataset/Revenue.aql";
    Lexer perloc_lexer = Lexer(perloc_aql_file_path);
    Lexer revenue_lexer = Lexer(revenue_aql_file_path);
    char Perloc[] = "../dataset/perloc/PerLoc.input";
    char Revenue[] = "../dataset/revenue/Revenue.input";
    char perloc_output_file_path[] = "Perloc.output";
    char revenue_output_file_path[] = "Revenue.output";
    Tokenizer tester1(Perloc);
    Tokenizer tester2(Revenue);
    Parser parser1(perloc_lexer, tester1, perloc_output_file_path, Perloc);
    Parser parser2(revenue_lexer, tester2, revenue_output_file_path, Revenue);
    parser1.program();
    parser2.program();
    return 0;
}

