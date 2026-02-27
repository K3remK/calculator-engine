#pragma once

#include <string>
#include <cmath>

namespace util
{
    static inline bool is_digit(char c)
    {
        return ('0' <= c && '9' >= c);
    }

    static inline double get_number(const std::string &str, std::size_t &index)
    {
        double number = 0;
        while (is_digit(str[index]))
        {
            int digit = static_cast<int>(str[index++] - '0');
            number *= 10;
            number += digit;
        }
        if (str[index++] == '.')
        {
            std::size_t tmp = index;
            double afterDecimal = get_number(str, index);
            double div = std::pow(10, index - tmp);
            number += (afterDecimal / div);
        }
        index -= 2;
        return number;
    }
}