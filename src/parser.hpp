#ifndef PARSER_H
#define PARSER_H
#include <unordered_map>
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

    std::unordered_map<Symbol *, std::unordered_set<Terminal *> *> firstDict;
    std::unordered_map<Symbol *, std::unordered_set<Terminal *> *> followDict;
    // TODO: Implement destructor function to free allocated memory space for
    // firstDict, followDict, etc.

  private:
    // TODO: Resolve unresolved recursive follow() call.
    std::unordered_set<Terminal *> toResolveFollow(Symbol *rSym);
    std::unordered_set<Terminal *> &
    resolveFollow(Symbol *s, std::unordered_set<Terminal *> &followSet);
};

#endif // !PARSER_H
