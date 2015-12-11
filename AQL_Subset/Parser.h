#include "Lexer.h"
#include "Tokenizer.h"
#include <vector>
#include <string>
#include <map>

struct span {
    std::string value;
    int from, to;
};

struct col {
    std::string name;
    std::vector<span> spans;
};

struct view {
    std::string name;
    std::vector<col> cols;
};

class Parser {
    public:
        Parser(Lexer lexer, Tokenizer tokenizer);
        token scan();
        void match(std::string);
        void error(std::string str);
        void create_view(std::string name, std::vector<col> cols);
        void create_col(std::string name, std::vector<span> spans);
        void create_span(std::string value, int from, int to);
        void output_view(view v);
        void program();
        void aql_stmt();
        void create_stmt();
        void view_stmt();
        void output_stmt();
        void alias();
        void select_stmt();
        void select_list();
        void select_item();
        void from_list();
        void from_item();
        void extract_stmt();
        void extract_spec();
        void regex_spec();
        void column();
        void name_spec();
        void group_spec();
        void single_group();
        void pattern_spec();
        void pattern_expr();
        void pattern_pkg();
        void atom();
        void pattern_group();
    private:
        std::vector<token> lexer_tokens;
        std::vector<std::string> document_tokens;
        token look;
        int lexer_parser_pos;
        std::map<std::string, view> views;
};

