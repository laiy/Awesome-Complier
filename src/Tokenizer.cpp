#include <cstdio>
#include <vector>
#include <string>
#include "Tokenizer.h"

Tokenizer::Tokenizer(const char *file_path) {
    FILE *original_document = fopen(file_path, "r");
    int token_size_count = 0, pos = -1;
    char c;
    std::string buffer;
    this->content = "";
    while ((c = fgetc(original_document)) != EOF) {
        pos++;
        this->content += c;
        if (is_digit(c) || is_letter(c))
            buffer += c, token_size_count++;
        else {
            if (token_size_count) {
                this->tokens.push_back(document_token(buffer, pos - buffer.length(), pos));
                buffer = "";
                token_size_count = 0;
            }
            if (c != ' ' && c != '\n' && c != '\r' && c != '\t')
                this->tokens.push_back(document_token(char_to_string(c), pos, pos + 1));
        }
    }
    if (token_size_count)
        pos++, this->tokens.push_back(document_token(buffer, pos - buffer.length(), pos));
}

std::vector<document_token> Tokenizer::get_tokens() {
    return this->tokens;
}

std::string Tokenizer::get_text() {
    return this->content;
}

inline bool is_digit(char c) {
    int temp = c - '0';
    return temp >= 0 && temp <= 9;
}

inline bool is_letter(char c) {
    int temp = c - 'a';
    return (temp >= 0) ? (temp <= 25) : (temp = c - 'A') >= 0 && temp <= 25;
}

inline std::string char_to_string(char c) {
    std::string str;
    return (str += c);
}

