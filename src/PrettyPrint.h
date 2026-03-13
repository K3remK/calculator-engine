//
// Created by kerem on 3/5/26.
//
#ifndef CALCULATOR_ENGINE_PRETTYPRINT_H
#define CALCULATOR_ENGINE_PRETTYPRINT_H

#include <vector>
#include <string>
#include <iomanip>


#include "Matrix.hpp"
#include "Token.hpp"

struct TextBlock {
    std::vector<std::string> lines;
    [[nodiscard]] size_t height() const { return lines.size(); }
    [[nodiscard]] size_t width() const { return lines.empty() ? 0 : lines[0].length(); }
};

class PrettyPrint {
    size_t height = 1;
public:
    static void print(const std::vector<Token>& tokens);
private:
    static void printEquation(const std::vector<TextBlock>& blocks);
    [[nodiscard]] TextBlock operatorToBlock(const std::string& op) const;
    static TextBlock matrixToBlock(const std::unique_ptr<Matrix<double>>& m);
    static std::string formatDouble(double value);
};


#endif //CALCULATOR_ENGINE_PRETTYPRINT_H