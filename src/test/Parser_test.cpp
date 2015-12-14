#include "../Parser.h"
#include <iostream>

int main() {
    char perloc_aql_file_path[] = "../dataset/PerLoc.aql";
    char revenue_aql_file_path[] = "../dataset/Revenue.aql";
    char html_aql_file_path[] = "../dataset/html.aql";
    Lexer perloc_lexer = Lexer(perloc_aql_file_path);
    Lexer revenue_lexer = Lexer(revenue_aql_file_path);
    Lexer html_lexer = Lexer(html_aql_file_path);
    char Perloc[] = "../dataset/perloc/PerLoc.input";
    char Revenue[] = "../dataset/revenue/Revenue.input";
    char laiy_github_profile_html[] = "../dataset/html/laiy_github_profile.html";
    char youtube_html[] = "../dataset/html/youtube.html";
    char perloc_output_file_path[] = "../dataset/Perloc.output";
    char revenue_output_file_path[] = "../dataset/Revenue.output";
    char laiy_github_profile_html_output_file_path[] = "../dataset/html/laiy_github_profile.html.output";
    char youtube_html_output_file_path[] = "../dataset/html/youtube.html.output";
    Tokenizer tester1(Perloc);
    Tokenizer tester2(Revenue);
    Tokenizer tester3(laiy_github_profile_html);
    Tokenizer tester4(youtube_html);
    Parser parser1(perloc_lexer, tester1, perloc_output_file_path, Perloc);
    Parser parser2(revenue_lexer, tester2, revenue_output_file_path, Revenue);
    Parser parser3(html_lexer, tester3, laiy_github_profile_html_output_file_path, laiy_github_profile_html);
    Parser parser4(html_lexer, tester4, youtube_html_output_file_path, youtube_html);
    parser1.program();
    parser2.program();
    parser3.program();
    parser4.program();
    return 0;
}

