#include <vector>
#include <string>

#define MAX_LEN 10000000

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
        std::string get_text();
    private:
        std::vector<document_token> tokens;
        std::string content;
};

inline bool is_digit(char c);

inline bool is_letter(char c);

inline std::string char_to_string(char c);

