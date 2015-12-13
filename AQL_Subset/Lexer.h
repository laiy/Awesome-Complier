#include <vector>
#include <string>

typedef enum {
    CREATE, VIEW, AS, OUTPUT, SELECT, FROM, EXTRACT, REGEX, ON, RETURN,
    GROUP, AND, TOKEN, PATTERN, ID, DOT, REG, NUM, LESSTHAN, GREATERTHAN,
    LEFTBRACKET, RIGHTBRACKET, CURLYLEFTBRACKET, CURLYRIGHTBRACKET, SEMICOLON, COMMA, END, EMPTY
} Type;

struct token {
    std::string value;
    Type type;
    bool is_grouped;
    token(std::string value, Type type) {
        this->value = value;
        this->type = type;
        this->is_grouped = false;
    }
    bool operator==(const token &t) const {
        return this->value == t.value && this->type == t.type;
    }
};

class Lexer {
    public:
        Lexer(char *file_path);
        std::vector<token> get_tokens();
    private:
        std::vector<token> tokens;
};

token get_token(std::string str);

