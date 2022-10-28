#include "cli_parser.hpp"
#include "grammar.hpp"
#include "parser.hpp"
#include "rule.hpp"
#include "symbol.hpp"

int main(int argc, char **argv) {
    CliParser clip(argc, argv);
    Grammar grammar;
    //    Parser parser;

    if (clip.g()) {
        // TODO: handle conflict with p
        grammar.loadGrammar(clip.gFile());
        if (clip.v()) {
            grammar.printRules();
        }
        //        parser.buildTable();
    }
}
