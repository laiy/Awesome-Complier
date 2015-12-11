#include "Lexer.h"
#include "Tokenizer.h"
#include <vector>
#include <string>
#include <map>

struct span {
    std::string value;
    int from, to;
    span(std::string value, int from, int to) {
        this->value = value;
        this->from = from;
        this->to = to;
    }
};

struct col {
    std::string name;
    std::vector<span> spans;
    col(std::string name, std::vector<span> spans) {
        this->name = name;
        this->spans = spans;
    }
};

struct view {
    std::string name;
    std::vector<col> cols;
    view(std::string name, std::vector<col> cols) {
        this->name = name;
        this->cols = cols;
    }
};

class Parser {
    public:
        Parser(Lexer lexer, Tokenizer tokenizer);
        token scan();
        void match(std::string);
        void error(std::string str);
        void output_view(view v);
        void program();
        void aql_stmt();
        void create_stmt();
        std::vector<col>& view_stmt();
        void output_stmt();
        token alias();
        std::vector<col> select_stmt();
        std::vector<token> select_list();
        std::vector<token> select_item();
        std::vector<token> from_list();
        std::vector<token> from_item();
        std::vector<col> extract_stmt();
        std::vector<token> extract_spec();
        std::vector<token> regex_spec();
        std::vector<token> column();
        std::vector<token> name_spec();
        std::vector<token> group_spec();
        std::vector<token> single_group();
        std::vector<token> pattern_spec();
        std::vector<token> pattern_expr();
        std::vector<token> pattern_pkg();
        std::vector<token> atom();
        std::vector<token> pattern_group();
    private:
        std::vector<token> lexer_tokens;
        std::vector<std::string> document_tokens;
        int lexer_parser_pos;
        token look;
        std::map<std::string, view> views;
};

