#include <iostream>
#include "../Lexer.h"

static const char *TYPE_STRING[] = {
    "CREATE", "VIEW", "AS", "OUTPUT", "SELECT", "FROM", "EXTRACT", "REGEX", "ON", "RETURN",
    "GROUP", "AND", "TOKEN", "PATTERN", "ID", "DOT", "REG", "NUM", "LESSTHAN", "GREATERTHAN",
    "LEFTBRACKET", "RIGHTBRACKET", "CURLYLEFTBRACKET", "CURLYRIGHTBRACKET", "SEMICOLON", "COMMA"
};

int main() {
    char perloc_aql_file_path[] = "../dataset/PerLoc.aql";
    char revenue_aql_file_path[] = "../dataset/Revenue.aql";
    Lexer perloc_lexer = Lexer(perloc_aql_file_path);
    Lexer revenue_lexer = Lexer(revenue_aql_file_path);
    std::vector<token> v = perloc_lexer.get_tokens();
    std::cout << "======================Perloc======================" << std::endl;
    for (int i = 0; (size_t)i < v.size(); i++)
        std::cout << v[i].value << " " << TYPE_STRING[v[i].type] << std::endl;
    v.clear();
    v = revenue_lexer.get_tokens();
    std::cout << "======================Revenue=====================" << std::endl;
    for (int i = 0; (size_t)i < v.size(); i++)
        std::cout << v[i].value << " " << TYPE_STRING[v[i].type] << std::endl;
    return 0;
}

