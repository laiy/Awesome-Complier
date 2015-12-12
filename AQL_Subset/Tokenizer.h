#include <vector>
#include <string>

struct document_token {
    std::string value;
    int from, to;
    document_token(std::string value, int from, int to) {
        this->value = value;
        this->from = from;
        this->to = to;
    }
};

class Tokenizer {
    public:
        Tokenizer(char *file_path);
        std::vector<document_token> get_tokens();
    private:
        std::vector<document_token> tokens;
};

inline bool is_digit(char c);

inline bool is_letter(char c);

inline std::string char_to_string(char c);

