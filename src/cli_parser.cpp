#include "cli_parser.hpp"
#include <iostream>

using namespace std;

CliParser::CliParser(int argc, char **argv) : argc(argc), argv(argv) {
    errorFlag = false;
    optg = opto = opti = optv = opts = false;
    for (int i = 0; i < argc; i++) {
        if (hasParameter("-h", i)) {
            printHelp();
            break;
        } else if (hasParameter("-g", i)) {
            optg = true;
            isHasRestParameter(i);
            gfile = argv[++i];
        } else if (hasParameter("-o", i)) {
            opto = true;
            isHasRestParameter(i);
            ofile = argv[++i];
        } else if (hasParameter("-p", i)) {
            optp = true;
            isHasRestParameter(i);
            pfile = argv[++i];
        } else if (hasParameter("-i", i)) {
            opti = true;
            isHasRestParameter(i);
            ifile = argv[++i];
        } else if (hasParameter("-s", i)) {
            opts = true;
            isHasRestParameter(i);
            sstr = argv[++i];
        } else if (hasParameter("-v", i)) {
            optv = true;
        } else {
            reportError();
            break;
        }
    }
}

void CliParser::isHasRestParameter(int i) {
    if (i + 1 > argc) {
        reportError();
    }
}

void CliParser::reportError() {
    errorFlag = true;
    printError();
}

void CliParser::printError() { cout << "Invalid option[s]?. Use -h for help"
<< endl; }

void CliParser::printHelp() {
    cout
        << "Options:\n"
           "-h: show options\n"
           "-g <filename>: generate a parser from the given grammar\n"
           "-o <filename>: save generated parser to <filename>\n"
           "-p <filename>: use the given parser file to parse\n"
           "-i <filename>: parse <filename>\n"
           "-s \"<input string>\": parse given string\n"
           "-v: give more output information\n"
           "\n"
           "Examples:\n"
           "generate a parser from grammar.txt and use it to parse input.txt:\n"
           "    ./parser -g grammar.txt -i input.txt\n"
           "generate a parser from grammar.txt:\n"
           "    ./parser -g grammer.txt -o parser_instance\n"
           "parse \"input string\" using parser_instance saved previously:\n"
           "    ./parser -p parser_instance -i \"input string\"\n"
           "parse input.txt using parser_instance saved previously:\n"
           "    ./parse -p parser_instance -i input.txt"
        << endl;
}

bool CliParser::hasParameter(const char* param, int i) {
    return strcmp(argv[i], param);
}
