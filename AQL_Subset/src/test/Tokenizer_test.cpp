#include "../Tokenizer.h"
#include <string>
#include <vector>
#include <iostream>

int main () {
    char Perloc[] = "../dataset/perloc/PerLoc.input";
    char Revenue[] = "../dataset/revenue/Revenue.input";
    Tokenizer tester1(Perloc);
    Tokenizer tester2(Revenue);
    std::vector<document_token> v = tester1.get_tokens();
    std::cout << "======================Perloc======================" << std::endl;
    for (int i = 0; (size_t)i < v.size(); i++)
        std::cout << v[i].value << ": ["<< v[i].from << ", " << v[i].to << ")" << std::endl;
    v.clear();
    v = tester2.get_tokens();
    std::cout << "======================Revenue=====================" << std::endl;
    for (int i = 0; (size_t)i < v.size(); i++)
        std::cout << v[i].value << ": ["<< v[i].from << ", " << v[i].to << ")" << std::endl;
}

