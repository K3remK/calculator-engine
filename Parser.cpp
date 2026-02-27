#include "Parser.hpp"

#include <sstream>
#include <iostream>
#include "Util.hpp"

std::vector<Token> Parser::Parse(const std::string &text) const
{
    std::vector<Token> tokens;

    for (std::size_t i = 0; i < text.length(); i++)
    {
        char curr = text.at(i);
        if (curr == ' ')
            continue;
        std::cout << curr << std::endl;
        if (util::is_digit(curr))
        {
            double number = util::get_number(text, i);
            tokens.push_back(Token(number));
        }
        else
        {
            tokens.push_back(Token(curr));
        }
    }

    return tokens;
}