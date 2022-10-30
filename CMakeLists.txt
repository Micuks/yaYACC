cmake_minimum_required(VERSION 3.14)
project(yaYACC)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
endif()
# set(CMAKE_BUILD_TYPE Release)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_RULE")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_GRAMMAR")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_SYMBOL")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_PARSER")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_LEX")

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
        tests/test_grammar.cpp
        tests/test_lex.cpp
        tests/test_parser.cpp
        )

# target_compile_definitions(
#         test_all
#         PRIVATE
#         $<$<CONFIG:Debug>:DEBUG>
#         )

target_link_libraries(
        test_all
        yaYACC
        GTest::gtest_main
)

include(googletest)
gtest_discover_tests(test_all)

# main executable part
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
        src/symbol.cpp
        src/rule.cpp
        src/grammar.cpp
        src/parser.cpp
        src/lex.cpp
        )
