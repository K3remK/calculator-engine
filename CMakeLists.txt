cmake_minimum_required(VERSION 3.10.0)
project(calculator-backend VERSION 0.1.0 LANGUAGES C CXX)

add_executable(calculator-backend 
                main.cpp
                Lexer.cpp
        Lexer.h
)

include(CTest)
enable_testing()

