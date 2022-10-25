#include "cli_parser.hpp"

int main(int argc, char **argv) {
    CliParser clip(argc, argv);
    Grammer grammer;
    Parser parser;

    if (clip.g()) {
        // TODO: handle conflict with p
        grammer.loadGrammer(clip.gFile());
        if(clip.v()){
            grammer.printRules();
        }
        parser.buildTable();
    }
}
