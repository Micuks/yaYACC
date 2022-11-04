#include "../src/cli_parser.hpp"
#include "../src/grammar.hpp"
#include "../src/lex.hpp"
#include "../src/main.hpp"
#include "../src/parser.hpp"
#include "../src/rule.hpp"
#include "../src/symbol.hpp"
#include <gtest/gtest.h>
#include <vector>

TEST(MainTest, mainTest) {
    int argc = 6;
    const char *argv[] = {
        "./main", "-s", "aaa bbb a", "-g", "../grammars/g1.txt", "-v"};
    CliParser clip(argc, argv);
    Grammar *grammar;
    Parser *parser;
    Lex *lex;
    std::string rawInput;
    std::vector<Terminal *> *tokenizedInput;

    // Load grammar
    if (clip.g()) {
        grammar = new Grammar();
        // Generate a parser from the given grammar.
        // TODO: handle conflict with p
        try {
            grammar->loadGrammar(clip.gFile());
        } catch (std::exception &e) {
            std::cerr << e.what();
            exit(-1);
        }
        if (clip.v()) {
            grammar->printRules();
            std::cout << "Grammar variables:\n";
            for (auto &a : grammar->variables) {
                std::cout << *a << ", ";
            }
            std::cout << std::endl;
            std::cout << "Grammar terminals:\n";
            for (auto &a : grammar->terminals) {
                std::cout << *a << ", ";
            }
            std::cout << std::endl;
        }

        parser = new Parser(grammar, clip.v());
        parser->makeTable();

        lex = new Lex(grammar, clip.v());

        if (clip.v()) {
            parser->printFirstTable();
            parser->printFollowTable();

            parser->printParseTable();
        }
    }
    // Load input string
    if (clip.s()) {
        // Parse given string
        rawInput = clip.sStr();
#ifdef DEBUG_MAIN
        std::cout << "Given string: " << rawInput;
#endif // DEBUG_MAIN
        tokenizedInput = lex->tokenize(rawInput);
    }

    try {
        parser->parse(tokenizedInput);
    } catch (std::exception &e) {
        std::cerr << e.what();
    }

    // Free allocated memory.
    delete tokenizedInput;
    delete grammar;
    delete parser;
    delete lex;
}
