#include "main.hpp"
#include "cli_parser.hpp"
#include "grammar.hpp"
#include "lex.hpp"
#include "parser.hpp"
#include "rule.hpp"
#include "symbol.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>

int main(int argc, const char **argv) {
    CliParser clip(argc, argv);
    Grammar *grammar;
    Parser *parser;
    Lex *lex;
    std::string rawInput;
    std::vector<Terminal *> *tokenizedInput;

    // Load config params
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
            std::cout << std::endl;
        }

        parser = new Parser(grammar, clip.v());
        parser->makeTable();

        lex = new Lex(grammar, clip.v());

        if (clip.v()) {
            parser->printFirstTable();
            std::cout << std::endl;
            parser->printFollowTable();
            std::cout << std::endl;
            parser->printParseTable();
            std::cout << std::endl;
        }
    }

    // Parse string params
    // Load input string
    if (clip.s()) {
        // Parse given string
        rawInput = clip.sStr();
        if (clip.v())
            std::cout << "Given string: " << rawInput;
        tokenizedInput = lex->tokenize(rawInput);
    }

    if (clip.i()) {
        // Parse string from given file
        if (clip.s()) {
            throw std::runtime_error(
                "ERROR: Cannot load string from file and command line "
                "interface by specifying -s and -i at the same time.");
            exit(-1);
        }

        std::ifstream f;
        try {
            f.open(clip.iFile());
        } catch (std::exception &e) {
            throw std::runtime_error("ERROR: Failed to open " + clip.iFile() +
                                     ". \n" + e.what());
        }
        rawInput = std::string(std::istreambuf_iterator<char>(f),
                               std::istreambuf_iterator<char>());

        if (clip.v()) {
            std::cout << "Given string from file:\n" << rawInput;
        }

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
    return 0;
}
