#include <vector>
#include <string>

typedef enum {
    CREATE, VIEW, AS, OUTPUT, SELECT, FROM, EXTRACT, REGEX, ON, RETURN,
    GROUP, AND, TOKEN, PATTERN, ID, DOT, REG, NUM, LESSTHAN, GREATERTHAN,
    LEFTBRACKET, RIGHTBRACKET, CURLYLEFTBRACKET, CURLYRIGHTBRACKET, SEMICOLON, COMMA
} Type;

struct token {
    std::string value;
    Type type;
    token(std::string value, Type type) {
        this->value = value;
        this->type = type;
    }
};

class Lexer {
    public:
        Lexer(char *file_path);
        std::vector<token> get_tokens();
    private:
        std::vector<token> tokens;
};

