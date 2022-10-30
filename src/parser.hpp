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
    Parser(Grammar *g, bool verbose)
        : grammar(g), verbose(verbose), lenParseTable(0) {}

    void parse(std::vector<Terminal *> *input);

    void makeTable();
    void dropTable();

    void importFromFile(const char *filename);
    void exportToFile(const char *filename);

    void printFirstTable();
    void printFollowTable();
    void printParseTable();

    // private:
    std::unordered_set<Terminal *> first(Symbol *s);
    std::unordered_set<Terminal *> follow(Symbol *s);

    // TODO: Resolve unresolved recursive follow() call.
    std::unordered_set<Terminal *> toResolveFollow(Symbol *rSym);
    std::unordered_set<Terminal *> &
    resolveFollow(Symbol *s, std::unordered_set<Terminal *> &followSet);

    int **parseTable;
    int lenParseTable;
    Grammar *grammar;
    bool verbose;

    std::unordered_map<Symbol *, std::unordered_set<Terminal *> *> firstDict;
    std::unordered_map<Symbol *, std::unordered_set<Terminal *> *> followDict;
    // TODO: Implement destructor function to free allocated memory space for
    // firstDict, followDict, etc.
};

#endif // !PARSER_H
