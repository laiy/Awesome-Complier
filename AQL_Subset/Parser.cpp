#include "Parser.h"
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <hash_map>
#include <cstring>

inline std::string char_to_string(char *content);

Parser::Parser(Lexer lexer, Tokenizer tokenizer) : lexer_tokens(lexer.get_tokens()),
    lexer_parser_pos(0), look(this->scan()), document_tokens(tokenizer.get_tokens()) {
        view v = view("Document");
        col c = col("text");
        span s = span(char_to_string(lexer.get_text()), 0, strlen(lexer.get_text()) - 1);
        c.spans.push_back(s);
        v.cols.push_back(c);
        this->views.push_back(v);
}

token Parser::scan() {
    if ((size_t)this->lexer_parser_pos < this->lexer_tokens.size())
        return this->lexer_tokens[this->lexer_parser_pos++];
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

void Parser::output_view(view v, std::string alias_name) {
    // output view with view and its alias
    // if alias is EMPTY token means there is no name for the alias
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
    else
        this->output_stmt();
    this->match(";");
}

void Parser::create_stmt() {
    this->match("create");
    this->match("view");
    std::string view_name = this->look.value;
    this->look = this->scan();
    this->match("as");
    std::vector<col> view_cols = this->view_stmt();
    view v = view(view_name);
    v.cols = view_cols;
    this->views.push_back(v);
}

std::vector<col> Parser::view_stmt() {
    if (this->look.type == SELECT)
        return this->select_stmt();
    else
        return this->extract_stmt();
}

void Parser::output_stmt() {
    this->match("output");
    this->match("view");
    std::string output_view_name = this->look.value;
    this->look = this->scan();
    token alias_name = this->alias();
    for (int i = 0; (size_t)i < this->views.size(); i++)
        if (this->views[i].name == output_view_name) {
            output_view(this->views[i], alias_name.value);
            break;
        }
}

token Parser::alias() {
    if (this->look.type == AS) {
        this->match("as");
        token t = this->look;
        this->look = this->scan();
        return t;
    } else {
        return token("", EMPTY);
    }
}

std::vector<col> Parser::select_stmt() {
    this->match("select");
    std::vector<token> select_list_v = this->select_list();
    this->match("from");
    std::vector<token> from_list_v = this->from_list();
    // create cols with select_list_v and from_list_v and return a vector<col>
    // todo
    std::vector<col> select_stmt_col_v;
    __gnu_cxx::hash_map<std::string, std::string> temp_to_origin_view_name;
    for (int i = 0; (size_t)i < from_list_v.size(); i += 2)
        temp_to_origin_view_name[from_list_v[i + 1].value] = from_list_v[i].value;
    for (int i = 0; (size_t)i < select_list_v.size(); i += 3)
        select_stmt_col_v.push_back(this->get_col(this->get_view(temp_to_origin_view_name[select_list_v[i].value]), select_list_v[i + 1].value));
    return select_stmt_col_v;
}

std::vector<token> Parser::select_list() {
    std::vector<token> select_list_v;
    while (true) {
        std::vector<token> select_item_v = this->select_item();
        select_list_v.insert(select_list_v.end(), select_item_v.begin(), select_item_v.end());
        if (this->look.type == COMMA)
            this->match(",");
        else
            break;
    }
    return select_list_v;
}

std::vector<token> Parser::select_item() {
    std::vector<token> select_item_v;
    select_item_v.push_back(this->look);
    this->look = this->scan();
    this->match(".");
    select_item_v.push_back(this->look);
    this->look = this->scan();
    select_item_v.push_back(this->alias());
    return select_item_v;
}

std::vector<token> Parser::from_list() {
    std::vector<token> from_list_v;
    while (true) {
        std::vector<token> from_item_v = this->from_item();
        from_list_v.insert(from_list_v.end(), from_item_v.begin(), from_item_v.end());
        if (this->look.type == COMMA)
            this->match(",");
        else
            break;
    }
    return from_list_v;
}

std::vector<token> Parser::from_item() {
    std::vector<token> from_item_v;
    from_item_v.push_back(this->look);
    this->look = this->scan();
    from_item_v.push_back(this->look);
    this->look = this->scan();
    return from_item_v;
}

std::vector<col> Parser::extract_stmt() {
    this->match("extract");
    std::vector<token> extract_spec_v = this->extract_spec();
    this->match("from");
    std::vector<token> from_list_v = this->from_list();
    // create cols and return vector<col>
    // todo
    __gnu_cxx::hash_map<std::string, std::string> temp_to_origin_view_name;
    for (int i = 0; (size_t)i < from_list_v.size(); i += 2)
        temp_to_origin_view_name[from_list_v[i + 1].value] = from_list_v[i].value;
    if (extract_spec_v[0].type == EMPTY) {
        std::string reg = extract_spec_v[1].value;
        col col_to_exec = this->get_col(this->get_view(temp_to_origin_view_name[extract_spec_v[2].value]), extract_spec_v[3].value);
        std::string col_name = (extract_spec_v.size() == 5) ? extract_spec_v[4].value : extract_spec_v[5].value;
    } else {
    }
}

std::vector<token> Parser::extract_spec() {
    if (this->look.type == REGEX)
        return this->regex_spec();
    else
        return this->pattern_spec();
}

std::vector<token> Parser::regex_spec() {
    std::vector<token> regex_spec_v;
    regex_spec_v.push_back(token("", EMPTY));
    this->match("regex");
    regex_spec_v.push_back(this->look);
    this->look = this->scan();
    this->match("on");
    std::vector<token> column_v = this->column();
    regex_spec_v.insert(regex_spec_v.end(), column_v.begin(), column_v.end());
    std::vector<token> name_spec_v = this->name_spec();
    regex_spec_v.insert(regex_spec_v.end(), name_spec_v.begin(), name_spec_v.end());
    return regex_spec_v;
}

std::vector<token> Parser::column() {
    std::vector<token> column_v;
    column_v.push_back(this->look);
    this->look = this->scan();
    this->match(".");
    column_v.push_back(this->look);
    this->look = this->scan();
    return column_v;
}

std::vector<token> Parser::name_spec() {
    std::vector<token> name_spec_v;
    if (this->look.type == AS)
        this->match("as"), name_spec_v.push_back(this->look), this->look = this->scan();
    else
        this->match("return"), name_spec_v = this->group_spec();
    return name_spec_v;
}

std::vector<token> Parser::group_spec() {
    std::vector<token> group_spec_v;
    while (true) {
        std::vector<token> single_group_v = this->single_group();
        group_spec_v.insert(group_spec_v.end(), single_group_v.begin(), single_group_v.end());
        if (this->look.type == AND)
            this->match("and");
        else
            break;
    }
    return group_spec_v;
}

std::vector<token> Parser::single_group() {
    std::vector<token> single_group_v;
    this->match("group");
    single_group_v.push_back(this->look);
    this->look = this->scan();
    this->match("as");
    single_group_v.push_back(this->look);
    this->look = this->scan();
    return single_group_v;
}

std::vector<token> Parser::pattern_spec() {
    std::vector<token> pattern_spec_v;
    this->match("pattern");
    std::vector<token> pattern_expr_v = this->pattern_expr();
    pattern_spec_v.insert(pattern_spec_v.end(), pattern_expr_v.begin(), pattern_expr_v.end());
    std::vector<token> name_spec_v = this->name_spec();
    pattern_spec_v.push_back(token("", EMPTY));
    pattern_spec_v.insert(pattern_spec_v.end(), name_spec_v.begin(), name_spec_v.end());
    return pattern_spec_v;
}

std::vector<token> Parser::pattern_expr() {
    std::vector<token> pattern_expr_v;
    while (true) {
        std::vector<token> pattern_pkg_v = this->pattern_pkg();
        pattern_expr_v.insert(pattern_expr_v.end(), pattern_pkg_v.begin(), pattern_pkg_v.end());
        if (this->look.type == LEFTBRACKET || this->look.type == TOKEN || this->look.type == REG || this->look.type == LESSTHAN)
            continue;
        else
            break;
    }
    return pattern_expr_v;
}

std::vector<token> Parser::pattern_pkg() {
    if (this->look.type == LEFTBRACKET)
        return this->pattern_group();
    std::vector<token> pattern_pkg_v;
    std::vector<token> atom_v = this->atom();
    pattern_pkg_v.insert(pattern_pkg_v.end(), atom_v.begin(), atom_v.end());
    if (this->look.type == CURLYLEFTBRACKET)
        this->match("{"), pattern_pkg_v.push_back(this->look), this->look = this->scan(), this->match(","),
            pattern_pkg_v.push_back(this->look), this->look = this->scan(), this->match("}");
    return pattern_pkg_v;
}

std::vector<token> Parser::atom() {
    std::vector<token> atom_v;
    if (this->look.type == REG)
        atom_v.push_back(this->look), this->look = this->scan();
    else {
        this->match("<");
        if (this->look.type == TOKEN)
            atom_v.push_back(this->look), this->look = this->scan();
        else {
            std::vector<token> column_v = this->column();
            atom_v.insert(atom_v.end(), column_v.begin(), column_v.end());
        }
        this->match(">");
    }
    return atom_v;
}

std::vector<token> Parser::pattern_group() {
    this->match("(");
    std::vector<token> pattern_group_v = this->pattern_expr();
    this->match(")");
    return pattern_group_v;
}

inline col Parser::get_col(view v, std::string col_name) {
    for (int i = 0; (size_t)i < v.cols.size(); i++)
        if (v.cols[i].name == col_name)
            return v.cols[i];
}

inline view Parser::get_view(std::string view_name) {
    for (int i = 0; (size_t)i < this->views.size(); i++)
        if (views[i].name == view_name)
            return views[i];
}

inline std::string char_to_string(char *content) {
    std::string result;
    for (int i = 0; (size_t)i < strlen(content); i++)
        result += content[i];
    return result;
}

