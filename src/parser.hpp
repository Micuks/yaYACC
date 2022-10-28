#ifndef PARSER_H
#define PARSER_H
#include <vector>

#include "grammar.hpp"
#include "symbol.hpp"
#include <stack>
#include <unordered_set>

class Parser {
  public:
    Parser() {}
    Parser(Grammar *g, bool verbose) : grammar(g), verbose(verbose) {}

    void parse(std::vector<Terminal *> *input);
    void makeTable();
    void importFromFile(const char *filename);
    void exportToFile(const char *filename);
    void printFirstTable();
    void printFollowTable();

    // private:
    int **parseTable;
    Grammar *grammar;
    bool verbose;

    std::unordered_set<Terminal *> first(Symbol *s);
    std::unordered_set<Terminal *> follow(Symbol *s);
};

#endif // !PARSER_H
