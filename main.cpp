#include <iostream>
#include <sstream>

#include "Lexer.h"
#include "Util.hpp"

int main()
{
    std::string s = " (  31  /  5  )  *  1023 12  ";

    const std::vector<Token> tokens = Lexer::Tokenize(s);

    for (auto &t : tokens)
        std::cout << t << std::endl;
    return 0;
}