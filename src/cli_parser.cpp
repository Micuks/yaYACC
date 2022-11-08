#include "cli_parser.hpp"
#include <iostream>

using namespace std;

CliParser::CliParser(int argc, const char **argv) : argc(argc), argv(argv) {
    errorFlag = false;
    // Skip argv[0]: <program name>
    if (argc == 1) {
        printHelp();
    }
    for (int i = 1; i < argc; i++) {
        if (hasParameter("-h", i)) {
            printHelp();
            break;
        } else if (hasParameter("-ll1", i)) {
            if (optlr1 == true) {
                std::cerr
                    << "ERROR: Unable to use -ll1 and -lr1 at the same time!\n";
                printHelp();
                break;
            }
            optll1 = true;
        } else if (hasParameter("-lr1", i)) {
            if (optll1 == true) {
                std::cerr
                    << "ERROR: Unable to use -ll1 and -lr1 at the same time!\n";
                printHelp();
                break;
            }
            optlr1 = true;
        } else if (hasParameter("-g", i)) {
            optg = true;
            isHasRestParameter(i);
            gfile = std::string(argv[++i]);
        } else if (hasParameter("-o", i)) {
            opto = true;
            isHasRestParameter(i);
            ofile = std::string(argv[++i]);
        } else if (hasParameter("-p", i)) {
            optp = true;
            isHasRestParameter(i);
            pfile = std::string(argv[++i]);
        } else if (hasParameter("-i", i)) {
            opti = true;
            isHasRestParameter(i);
            ifile = std::string(argv[++i]);
        } else if (hasParameter("-s", i)) {
            opts = true;
            isHasRestParameter(i);
            sstr = std::string(argv[++i]);
        } else if (hasParameter("-v", i)) {
            optv = true;
        } else {
            reportError();
            break;
        }
    }
    if (!optll1 && !optlr1) {
        printHelp();
        std::cout << std::endl;
        std::cerr << ("ERROR: You need to specify the parser you "
                      "want to use via -lr1 or -ll1.\n");
        exit(-1);
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

void CliParser::printError() {
    cout << "Invalid option[s]?. Use -h for help" << endl;
}

void CliParser::printHelp() {
    cout << "Options:\n"
            "-h: show options\n"
            "-g <filename>: generate a parser from the given grammar\n"
            "-ll1 : Use LL(1) parser\n"
            "-lr1 : Use LR(1) parser\n"
            "-i <filename>: parse <filename>\n"
            "-s \"<input string>\": parse given string\n"
            "-v: give more output information\n"
            "\n"
            "Examples:\n"
            "ll(1) parse \"input string\" using parser_instance saved "
            "previously:\n"
            "    ./parser -p parser_instance -i \"input string\" -ll1\n"
            "lr(1) parse input.txt using parser_instance saved previously:\n"
            "    ./parse -lr1 -p parser_instance -i input.txt"
         << endl;
}

bool CliParser::hasParameter(const char *param, int i) {
    return !strcmp(argv[i], param);
    // return std::string(argv[i]) == std::string(param);
}
