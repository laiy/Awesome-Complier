#include "Lexer.h"
#include "Tokenizer.h"
#include "Parser.h"
#include <vector>
#include <string>
#include <cstdio>

// struct span {
    // std::string value;
    // int from, to;
// };

// struct col {
    // std::string name;
    // std::vector<span> spans;
// };

// struct view {
    // std::string name;
    // std::vector<col> cols;
// };

// private:
    // std::vector<token> lexer_tokens;
    // std::vector<std::string> document_tokens;
    // token look;
    // int lexer_parser_pos;
    // std::map<std::string, view> views;

Parser::Parser(Lexer lexer, Tokenizer tokenizer) : lexer_tokens(lexer.get_tokens()),
    lexer_parser_pos(0), look(this->scan()), document_tokens(tokenizer.get_tokens()) {
        views.clear();
}

token Parser::scan() {
    if (lexer_parser_pos < lexer_tokens.size())
        return lexer_tokens[lexer_parser_pos++];
    return token("", END);
}

void Parser::match(std::string str) {
    if (this->look == get_token(str))
        this->look = this->scan();
    else
        this->error("match failed.");
}

void Parser::error(std::string str) {
    printf("%s\n", str.c_str());
    exit(2);
}

void Parser::output_view(view v) {
    // todo
}

void Parser::program() {
    token end_token = token("", END);
    while (true) {
        if (this->look == end_token)
            break;
        this->aql_stmt();
    }
}

void Parser::aql_stmt() {
    if (this->look.type == CREATE)
        this->create_stmt();
    this->output_stmt();
}

void Parser::create_stmt() {
    this->match("create");
    this->match("view");
    std::string view_name = this->look.value;
    this->look = this->scan();
    this->match("as");
    std::vector<col> view_cols = this->view_stmt();
    // create view with name and cols
}

std::vector<col>& Parser::view_stmt() {
    if (this->look.type == SELECT)
        return this->select_stmt();
    return this->extract_stmt();
}

void Parser::output_stmt() {
    this->match("output");
    this->match("view");
    std::string output_view_name = this->look.value;
    this->look = this->scan();
    token alias_name = this->alias();
    // output view with view name and its alias
    // if alias is EMPTY token means there is no name for the alias
}

token Parser::alias() {
    if (this->look.type == AS) {
        token t = look;
        this->match("as");
        this->look = this->scan();
        return t;
    } else {
        return token("", EMPTY);
    }
}

std::vector<col>& Parser::select_stmt() {
    this->match("select");
    std::vector<token> select_list_v = this->select_list();
    // repeat: ID ID (ID or not) if not, insert an EMPTY token
    this->match("from");
    select_list_v.push_back(token("", EMPTY));
    std::vector<token> from_list_v = this->from_list();
    from_list_v.insert(from_list_v.begin(), select_list_v.begin(), select_list_v.end());
    // final is select_list_v(3 * n) + EMPTY token + from_list_v(2 * n)
    return from_list_v;
}

std::vector<token>& Parser::select_list();
std::vector<token>& Parser::select_item();
std::vector<token>& Parser::from_list();
std::vector<token>& Parser::from_item();
std::vector<col>& Parser::extract_stmt();
std::vector<token>& Parser::extract_spec();
std::vector<token>& Parser::regex_spec();
std::vector<token>& Parser::column();
std::vector<token>& Parser::name_spec();
std::vector<token>& Parser::group_spec();
std::vector<token>& Parser::single_group();
std::vector<token>& Parser::pattern_spec();
std::vector<token>& Parser::pattern_expr();
std::vector<token>& Parser::pattern_pkg();
std::vector<token>& Parser::atom();
std::vector<token>& Parser::pattern_group();

