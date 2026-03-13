//
// Created by kerem on 3/5/26.
//

#include "PrettyPrint.h"
#include <iostream>
#include <sstream>

std::string PrettyPrint::formatDouble(double value) {
    std::ostringstream os;
    os << std::setprecision(10) << value;
    return os.str();
}

void PrettyPrint::print(const std::vector<Token> &tokens) {
    bool hasMatrix = false;
    size_t maxHeight = 1;

    for (const auto& t : tokens) {
        if (t.type & MatrixT) {
            hasMatrix = true;
            const auto& matrix = std::get<std::unique_ptr<Matrix<double>>>(t.data);
            if (matrix)
                maxHeight = std::max(maxHeight, matrix->GetM());
        }
    }

    if (!hasMatrix) {
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (i > 0) std::cout << ' ';
            std::cout << tokens[i].toString();
        }
        std::cout << "\n";
        return;
    }

    std::vector<TextBlock> blocks;
    blocks.reserve(tokens.size());
    PrettyPrint pp;
    pp.height = maxHeight;

    for (const auto& t : tokens) {
        if (t.type & MatrixT) {
            blocks.emplace_back(matrixToBlock(std::get<std::unique_ptr<Matrix<double>>>(t.data)));
        } else {
            blocks.emplace_back(pp.operatorToBlock(t.toString()));
        }
    }

    printEquation(blocks);
}

void PrettyPrint::printEquation(const std::vector<TextBlock> &blocks) {
    size_t maxHeight = 0;
    for (const auto& b : blocks) maxHeight = std::max(maxHeight, b.height());

    for (size_t i = 0; i < maxHeight; ++i) {
        for (const auto& b : blocks) {
            if (const size_t offset = (maxHeight - b.height()) / 2; i >= offset && (i - offset) < b.height()) {
                std::cout << b.lines[i - offset];
            } else {
                std::cout << std::string(b.width(), ' ');
            }
        }
        std::cout << "\n";
    }
}

TextBlock PrettyPrint::operatorToBlock(const std::string& op) const {
    TextBlock block;
    const size_t middle = height / 2;
    for (size_t i = 0; i < height; ++i) {
        block.lines.push_back(i == middle ? " " + op + " " : std::string(op.length() + 2, ' '));
    }
    return block;
}

TextBlock PrettyPrint::matrixToBlock(const std::unique_ptr<Matrix<double>> &m) {
    TextBlock block;
    if (!m) {
        block.lines.push_back("[ null ]");
        return block;
    }

    const auto& data = m->GetData();
    if (data.empty()) {
        block.lines.push_back("[ ]");
        return block;
    }

    int max_w = 0;
    for (const auto& row : data) {
        for (const double v : row) {
            const std::string s = formatDouble(v);
            max_w = std::max(max_w, static_cast<int>(s.length()));
        }
    }

    for (const auto& row : data) {
        std::string line = "[ ";
        for (size_t i = 0; i < row.size(); ++i) {
            const std::string s = formatDouble(row[i]);
            line += std::string(max_w - static_cast<int>(s.length()), ' ') + s;
            if (i + 1 < row.size()) line += "  ";
        }
        line += " ]";
        block.lines.push_back(line);
    }
    return block;
}
