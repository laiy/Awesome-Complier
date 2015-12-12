#include "Parser.h"
#include "regex.hpp"
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <map>
#include <cstring>

struct record {
    int to;
    std::vector<int> pos;
    record(int to) {
        this->to = to;
    }
};

Parser::Parser(Lexer lexer, Tokenizer tokenizer) : lexer_tokens(lexer.get_tokens()),
    lexer_parser_pos(0), look(this->scan()), document_tokens(tokenizer.get_tokens()) {
        view v = view("Document");
        col c = col("text");
        span s = span(tokenizer.get_text(), 0, tokenizer.get_text().length());
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
    std::cout << "view: " << v.name << std::endl;
    for (int i = 0; (size_t)i < v.cols.size(); i++) {
        std::cout << "    col: " << v.cols[i].name << std::endl;
        for (int j = 0; (size_t)j < v.cols[i].spans.size(); j++) {
            std::cout << "        spans: " << v.cols[i].spans[j].value << "from: " << v.cols[i].spans[j].from << "to: " << v.cols[i].spans[j].to << std::endl;
        }
    }
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
    std::map<std::string, std::string> temp_to_origin_view_name;
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
    std::map<std::string, std::string> temp_to_origin_view_name;
    for (int i = 0; (size_t)i < from_list_v.size(); i += 2)
        temp_to_origin_view_name[from_list_v[i + 1].value] = from_list_v[i].value;
    if (extract_spec_v[0].type == EMPTY) {
        // regex
        std::string reg = extract_spec_v[1].value.substr(1, extract_spec_v[1].value.length() - 2);
        col col_to_exec = this->get_col(this->get_view(temp_to_origin_view_name[extract_spec_v[2].value]), extract_spec_v[3].value);
        std::string col_name = (extract_spec_v.size() == 5) ? extract_spec_v[4].value : extract_spec_v[5].value;
        std::string document = col_to_exec.spans[0].value;
        std::vector< std::vector<int> > result = findall(reg.c_str(), document.c_str());
        std::vector<col> regex_spec_col_v;
        col regex_exec_result = col(col_name);
        for (int i = 0; (size_t)i < result.size(); i++) {
            std::string match;
            for (int j = result[i][0]; j < result[i][1]; j++)
                match += document[j];
            regex_exec_result.spans.push_back(span(match, result[i][0], result[i][1]));
        }
        regex_spec_col_v.push_back(regex_exec_result);
        return regex_spec_col_v;
    } else {
        // pattern
        int look = 0;
        std::vector<col> cols_to_exec;
        while (extract_spec_v[look].type != EMPTY) {
            if (extract_spec_v[look].type == ID) {
                col c = this->get_col(this->get_view(temp_to_origin_view_name[extract_spec_v[look].value]), extract_spec_v[look + 1].value);
                cols_to_exec.push_back(c);
                look += 2;
            } else if (extract_spec_v[look].type == TOKEN) {
                int min = atoi(extract_spec_v[look + 1].value.c_str()), max = atoi(extract_spec_v[look + 2].value.c_str());
                std::string document = this->get_col(this->get_view("Document"), "text").spans[0].value;
                col token_col = col("token");
                for (int i = min; i <= max; i++)
                    for (int j = 0; (size_t)j + i <= this->document_tokens.size(); j++)
                        token_col.spans.push_back(span("token", this->document_tokens[j].from, this->document_tokens[j + i - 1].to));
                cols_to_exec.push_back(token_col);
                look += 3;
            } else if (extract_spec_v[look].type == REG) {
                std::string reg = extract_spec_v[look].value.substr(1, extract_spec_v[look].value.length() - 2);
                std::string document = this->get_col(this->get_view("Document"), "text").spans[0].value;
                std::vector< std::vector<int> > result = findall(reg.c_str(), document.c_str());
                col regex_exec_result = col("regex");
                for (int i = 0; (size_t)i < result.size(); i++) {
                    std::string match;
                    for (int j = result[i][0]; j < result[i][1]; j++)
                        match += document[j];
                    regex_exec_result.spans.push_back(span(match, result[i][0], result[i][1]));
                }
                cols_to_exec.push_back(regex_exec_result);
                look++;
            } else {
                this->error("pattern match failed.");
            }
        }
        std::vector<record> r;
        std::string document = this->get_col(this->get_view("Document"), "text").spans[0].value;
        for (int j = 0; (size_t)j < cols_to_exec[0].spans.size(); j++) {
            int temp = cols_to_exec[0].spans[j].to;
            while (document[temp] == ' ')
                temp++;
            record re = record(temp);
            re.pos.push_back(j);
            r.push_back(re);
        }
        for (int i = 0; (size_t)i < cols_to_exec.size() - 1; i++) {
            std::vector<record> temp_record;
            for (int j = 0; (size_t)j < cols_to_exec[i + 1].spans.size(); j++) {
                for (int k = 0; (size_t)k < r.size(); k++) {
                    if (cols_to_exec[i + 1].spans[j].from == r[k].to) {
                        int temp = cols_to_exec[i + 1].spans[j].to;
                        while (document[temp] == ' ')
                            temp++;
                        record re = record(temp);
                        for (int t = 0; (size_t)t < r[k].pos.size(); t++)
                            re.pos.push_back(r[k].pos[t]);
                        re.pos.push_back(j);
                        temp_record.push_back(re);
                    }
                }
            }
            r.clear();
            r.insert(r.end(), temp_record.begin(), temp_record.end());
        }
        look++;
        std::vector<col> group;
        if (extract_spec_v[look].type == ID)
            group.push_back(col(extract_spec_v[look].value));
        else {
            while ((size_t)look < extract_spec_v.size())
                group.push_back(col(extract_spec_v[look + 1].value)), look += 2;
        }
        for (int i = 0; (size_t)i < r.size(); i++) {
            int group_count = 1;
            std::string span_value;
            for (int j = 0; (size_t)j < r[i].pos.size(); j++) {
                span s = cols_to_exec[j].spans[r[i].pos[j]];
                if (s.value == "token")
                    continue;
                else {
                    span_value += s.value;
                    if (cols_to_exec[j].name != "regex")
                        group[group_count++].spans.push_back(s);
                }
            }
            span left = cols_to_exec[0].spans[r[i].pos[0]];
            span right = cols_to_exec[cols_to_exec.size() - 1].spans[r[i].pos[cols_to_exec.size() - 1]];
            group[0].spans.push_back(span(span_value, left.from, right.to));
        }
        return group;
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
        if (this->views[i].name == view_name)
            return this->views[i];
}

