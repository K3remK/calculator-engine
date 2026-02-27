#include <iostream>
#include <sstream>

#include "Parser.hpp"
#include "Util.hpp"

int main()
{
    std::string s = " ( 31  + 5)  *   1023   ";
    Parser p;
    std::vector<Token> tokens = p.Parse(s);
    // std::size_t index = 1;
    // std::cout << util::get_number(s, index) << ", index: " << index << std::endl;

    for (auto &t : tokens)
        std::cout << t << std::endl;

    return 0;
}