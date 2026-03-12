//
// Created by kerem on 3/3/26.
//

#ifndef CALCULATOR_ENGINE_IO_H
#define CALCULATOR_ENGINE_IO_H

#ifdef _WIN32
    #include <conio.h> // Windows handles this easily with _getch()
#else
    #include <termios.h>
    #include <unistd.h>
#endif

#include <string>
#include <iostream>


enum Color {
    BLACK = 30,
    WHITE = 37,
};

static inline void print(const std::string& text, const Color bgColor, const Color fgColor) {
    std::cout << "\033[" + std::to_string(bgColor + 10) + "m\033[" + std::to_string(fgColor) + "m" << text << "\033[0m";
}

namespace io {
    static inline int getch() {
#ifdef _WIN32
        return _getch(); // On Windows, this is already non-canonical
#else
        termios oldt{}, newt{};
        tcgetattr(STDIN_FILENO, &oldt);           // Get current settings
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);        // Disable buffering and echoing
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply settings immediately

        const int ch = getchar();                          // Read one char

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore original settings
        return ch;
#endif
    }
}

#endif //CALCULATOR_ENGINE_IO_H