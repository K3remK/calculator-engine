//
// Created by kerem on 3/5/26.
//

#include "PrettyPrint.h"
#include <iostream>

void PrettyPrint::print(const std::vector<Token> &tokens) {
    // 1. First pass: Determine the global max height
    size_t maxHeight = 1;
    for (auto& t : tokens) {
        if (t.type & MatrixT) {
            maxHeight = std::max(maxHeight, std::get<Matrix<double>>(t.data).GetM());
        }
    }

    // 2. Second pass: Generate blocks using the global maxHeight
    std::vector<TextBlock> blocks;
    PrettyPrint pp;
    pp.height = maxHeight; // Set the instance height for operators

    for (auto& t : tokens) {
        if (t.type & MatrixT) {
            blocks.emplace_back(pp.matrixToBlock(std::get<Matrix<double>>(t.data)));
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
            // VERTICAL CENTERING LOGIC
            // Calculate an offset so short matrices sit in the middle of tall ones

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
    for (int i = 0; i < height; ++i) {
        block.lines.push_back(i == middle ? " " + op + " " : std::string(op.length() + 2, ' '));
    }
    return block;
}

TextBlock PrettyPrint::matrixToBlock(const Matrix<double> &m) {
    TextBlock block;
    const auto& data = m.GetData();
    if (data.empty()) return block;

    // 1. Find the widest number for column alignment
    int max_w = 0;
    for (auto& row : data) {
        for (const double v : row) {
            std::stringstream ss;
            ss << v; // Using stringstream to avoid the "1.000000" overkill
            max_w = std::max(max_w, static_cast<int>(ss.str().length()));
        }
    }

    // 2. Build each row with brackets [ ]
    for (const auto& row : data) {
        std::string line = "[ "; // Opening bracket
        for (size_t i = 0; i < row.size(); ++i) {
            std::stringstream ss;
            ss << row[i];
            std::string s = ss.str();

            // Right-align numbers within the column width
            line += std::string(max_w - s.length(), ' ') + s;

            // Add spacing between columns, but not after the last one
            if (i < row.size() - 1) line += "  ";
        }
        line += " ]"; // Closing bracket
        block.lines.push_back(line);
    }
    return block;
}
