#include <vector>
#include <string>
#include <cstdio>
#include "Lexer.h"
#include "Tokenizer.h"

token get_token(std::string str);
inline bool is_number(const std::string& s);

Lexer::Lexer(char *file_path) {
    Tokenizer tokenizer = Tokenizer(file_path);
    std::vector<document_token> document_tokens = tokenizer.get_tokens();
    std::string buffer;
    for (std::vector<document_token>::iterator it = document_tokens.begin(); it != document_tokens.end(); it++) {
        if (it->value == "/") {
            buffer = "/";
            std::string pre = "";
            int last_to = it->to, from = last_to - 1;
            it = document_tokens.erase(it);
            while (it->value != "/" || pre == "\\") {
                while (last_to != it->from)
                    buffer += ' ', last_to++;
                buffer += it->value, last_to = it->to, pre = it->value, it = document_tokens.erase(it);
            }
            int to = last_to + 1;
            buffer += "/";
            it = document_tokens.erase(it);
            it = document_tokens.insert(it, document_token(buffer, from, to));
        }
        this->tokens.push_back(get_token(it->value));
    }
}

std::vector<token> Lexer::get_tokens() {
    return this->tokens;
}

token get_token(std::string str) {
    Type type;
    if (str == ".")
        type = DOT;
    else if (str == ",")
        type = COMMA;
    else if (str == "<")
        type = LESSTHAN;
    else if (str == ">")
        type = GREATERTHAN;
    else if (str == "(")
        type = LEFTBRACKET;
    else if (str == ")")
        type = RIGHTBRACKET;
    else if (str == "{")
        type = CURLYLEFTBRACKET;
    else if (str == "}")
        type = CURLYRIGHTBRACKET;
    else if (str == ";")
        type = SEMICOLON;
    else if (str == "create")
        type = CREATE;
    else if (str == "view")
        type = VIEW;
    else if (str == "as")
        type = AS;
    else if (str == "output")
        type = OUTPUT;
    else if (str == "select")
        type = SELECT;
    else if (str == "from")
        type = FROM;
    else if (str == "extract")
        type = EXTRACT;
    else if (str == "regex")
        type = REGEX;
    else if (str == "on")
        type = ON;
    else if (str == "return")
        type = RETURN;
    else if (str == "group")
        type = GROUP;
    else if (str == "and")
        type = AND;
    else if (str == "Token")
        type = TOKEN;
    else if (str == "pattern")
        type = PATTERN;
    else if (is_number(str))
        type = NUM;
    else if (str[0] == '/' && str[str.length() - 1] == '/')
        type = REG;
    else
        type = ID;
    return token(str, type);
}

inline bool is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

