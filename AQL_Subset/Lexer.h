#include <vector>
#include <string>

#define MAX_LEN 1000000

typedef enum {
    CREATE, VIEW, AS, OUTPUT, SELECT, FROM, EXTRACT, REGEX, ON, RETURN,
    GROUP, AND, TOKEN, PATTERN, ID, DOT, REG, NUM, LESSTHAN, GREATERTHAN,
    LEFTBRACKET, RIGHTBRACKET, CURLYLEFTBRACKET, CURLYRIGHTBRACKET, SEMICOLON, COMMA, END, EMPTY
} Type;

struct token {
    std::string value;
    Type type;
    token(std::string value, Type type) {
        this->value = value;
        this->type = type;
    }
    bool operator==(const token &t) const {
        return this->value == t.value && this->type == t.type;
    }
};

class Lexer {
    public:
        Lexer(char *file_path);
        std::vector<token> get_tokens();
        char* get_text();
    private:
        std::vector<token> tokens;
        char content[MAX_LEN];
};

inline token get_token(std::string str);

inline bool is_number(const std::string& s);

