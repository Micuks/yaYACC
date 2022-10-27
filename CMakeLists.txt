cmake_minimum_required(VERSION 3.14)
project(yaYACC)

set(CMAKE_CXX_STANDARD 17)

include(fetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        test_all
        tests/test_all.cpp
        tests/test_symbol.cpp
        tests/test_rule.cpp
        tests/test_grammar.cpp)

target_link_libraries(
        test_all
        yaYACC
        GTest::gtest_main
)

include(googletest)
gtest_discover_tests(test_all)

add_executable(
        main
        src/main.cpp
)

target_link_libraries(
        main
        yaYACC
        )

add_library(
        yaYACC
        src/cli_parser.cpp
        src/grammar.cpp
#        src/parser.cpp
        src/rule.cpp
        src/symbol.cpp
        )
