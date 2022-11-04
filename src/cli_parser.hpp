#ifndef CLI_PARSER_HPP
#define CLI_PARSER_HPP

#include "main.hpp"
#include <string>

class CliParser {
  public:
    CliParser(int argc, const char **argv);
    bool g() { return optg; }
    std::string gFile() { return gfile; }
    bool o() { return opto; }
    std::string oFile() { return ofile; }
    bool p() { return optp; }
    std::string pFile() { return pfile; }
    bool i() { return opti; }
    std::string iFile() { return ifile; }
    bool v() { return optv; }
    bool s() { return opts; }
    std::string sStr() { return sstr; }

  private:
    int argc;
    const char **argv;
    bool errorFlag;
    bool optg, opto, optp, opti, optv, opts;
    std::string gfile, pfile, ofile, ifile, sstr;
    void printHelp();
    void printError();
    void reportError();
    void isHasRestParameter(int i);
    bool hasParameter(const char *param, int i);
};

#endif // !CLI_PARSER_HPP
