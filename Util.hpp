#pragma once

#include <string>
#include <cmath>

namespace util
{
    static double get_number(const std::string_view &str, std::size_t &index)
    {
        const std::size_t start = index;
        while (index < str.length() && (std::isdigit(str[index]) || str[index++] == '.'))
        {
            index++;
        }
        const std::string_view s = str.substr(start, index - start);
        return std::stod(std::string(s));
    }

    static const std::string& get_string(const std::string_view &str, std::size_t &index) {
        auto word = "";
        while (std::isalpha(str[index])) {
            word += str[index++];
        }
        index -= 2;
        return word;
    }
}