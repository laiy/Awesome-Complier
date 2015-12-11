#include <vector>
#include <string>

class Tokenizer {
    public:
        Tokenizer(char *file_path);
        std::vector<std::string> get_tokens();
    private:
        std::vector<std::string> tokens;
};

inline bool is_digit(char c);

inline bool is_letter(char c);

inline std::string char_to_string(char c);

