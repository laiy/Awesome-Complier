#include <cstdio>
#include <vector>
#include <string>
#include "Tokenizer.h"

inline bool is_digit(char c);
inline bool is_letter(char c);
inline std::string char_to_string(char c);

Tokenizer::Tokenizer(char *file_path) {
    FILE *original_document = fopen(file_path, "r");
    int token_size_count = 0;
    char c;
    std::string buffer;
    while ((c = fgetc(original_document)) != EOF) {
        if (is_digit(c) || is_letter(c))
            buffer += c, token_size_count++;
        else {
            if (token_size_count) {
                this->tokens.push_back(buffer);
                buffer = "";
                token_size_count = 0;
            }
            if (c != ' ' && c != '\n' && c != '\r' && c != '\t')
                this->tokens.push_back(char_to_string(c));
        }
    }
    if (token_size_count)
        this->tokens.push_back(buffer);
}

std::vector<std::string> Tokenizer::get_tokens() {
    return this->tokens;
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

