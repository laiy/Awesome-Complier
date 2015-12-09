#include <cstdio>
#include <vector>
#include <string>

class Tokenizer {
    public:
        Tokenizer(char *file_path);
        std::vector<std::string> get_tokens();
    private:
        std::vector<std::string> tokens;
};

