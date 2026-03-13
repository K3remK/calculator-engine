cmake_minimum_required(VERSION 3.14.0)
project(calculator-engine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF) # For portable, standard-compliant code

add_executable(calculator-engine
        src/main.cpp
        src/Lexer.cpp
        src/Lexer.hpp
        src/Parser.cpp
        src/Parser.hpp
        src/Evaluator.cpp
        src/Evaluator.hpp
        src/Token.hpp
        src/Matrix.cpp
        src/Matrix.hpp
        src/IO.h
        src/PrettyPrint.cpp
        src/PrettyPrint.h
)

# 1. Fetch GoogleTest from GitHub
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows: prevent gtest from overriding the runtime library settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 2. Add your library/logic
add_library(calculator_lib
    src/Lexer.cpp
    src/Parser.cpp
    src/Evaluator.cpp
    src/Token.hpp
    src/Evaluator.hpp
    src/IO.h
    src/Lexer.hpp
    src/Matrix.hpp
    src/Parser.hpp
    src/PrettyPrint.h
    src/Matrix.cpp
    src/PrettyPrint.cpp
)

# 3. Add the test executable
add_executable(unit_tests
    tests/test_main.cpp
    tests/test_token.cpp
    tests/test_lexer.cpp
    tests/test_parser.cpp
    tests/test_evaluator.cpp
    tests/test_integration.cpp
    tests/test_matrix.cpp
)

# 4. Link GTest and your library
target_link_libraries(unit_tests
        gtest_main
        calculator_lib
)

# 5. Enable 'ctest' command
include(GoogleTest)
gtest_discover_tests(unit_tests)

include(CTest)
enable_testing()

