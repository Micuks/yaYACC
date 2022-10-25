#ifndef CLI_PARSER_HPP
#define CLI_PARSER_HPP
#endif // !CLI_PARSER_HPP

#include <string>

class CliParser {
  public:
    CliParser(int argc, char **argv);
    bool g() { return optg; }
    char *gFile() { return gfile; }
    bool o() { return opto; }
    char *oFile() { return ofile; }
    bool p() { return optp; }
    char *pFile() { return pfile; }
    bool i() { return opti; }
    char *iFile() { return ifile; }
    bool v() { return optv; }
    bool s() { return opts; }
    char *sStr() { return sstr; }

  private:
    int argc;
    char **argv;
    bool errorFlag;
    bool optg, opto, optp, opti, optv, opts;
    char *gfile, *pfile, *ofile, *ifile, *sstr;
    void printHelp();
    void printError();
    void reportError();
    void isHasRestParameter(int i);
    bool hasParameter(std::string param);
};
