cmake_minimum_required(VERSION 3.10.0)
project(calculator-engine VERSION 0.1.0 LANGUAGES C CXX)

add_executable(calculator-engine 
        main.cpp
        Lexer.cpp
        Lexer.hpp
        Parser.cpp
        Parser.hpp
        Evaluator.cpp
        Evaluator.hpp
)

include(CTest)
enable_testing()

