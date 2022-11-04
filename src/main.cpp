#include "main.hpp"
#include "cli_parser.hpp"
#include "grammar.hpp"
#include "lex.hpp"
#include "parser.hpp"
#include "rule.hpp"
#include "symbol.hpp"
#include <vector>

int main(int argc, const char **argv) {
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
    return 0;
}
