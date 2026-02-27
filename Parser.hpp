#pragma once

#include "Token.hpp"
#include <vector>
#include <string>

class Parser
{
public:
    std::vector<Token> Parse(const std::string &text) const;
};