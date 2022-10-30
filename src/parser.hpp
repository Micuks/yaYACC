#ifndef PARSER_H
#define PARSER_H
#include "main.hpp"
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

    ~Parser() {
        dropTable();

        for (auto &[k, v] : firstDict) {
            delete v;
        }

        for (auto &[k, v] : followDict) {
            delete v;
        }
    }

    void parse(std::vector<Terminal *> *tokens);

    void makeTable();
    void dropTable();

    void importFromFile(const char *filename);
    void exportToFile(const char *filename);

    void printFirstTable();
    void printFollowTable();

    std::string parseTableToString();
    void printParseTable();

  private:
    std::unordered_set<Terminal *> first(Symbol *s);
    std::unordered_set<Terminal *> follow(Symbol *s);

    // TODO: Resolve unresolved recursive follow() call.
    std::unordered_set<Terminal *> toResolveFollow(Symbol *rSym);
    std::unordered_set<Terminal *> &
    resolveFollow(Symbol *s, std::unordered_set<Terminal *> &followSet);

    std::string stackToString(std::stack<Symbol *> pda);
    std::string remainingTokensToString(std::vector<Terminal *>::iterator it,
                                        std::vector<Terminal *> *tokens);

    int **parseTable;
    int lenParseTable;
    Grammar *grammar;
    bool verbose;

    std::unordered_map<Symbol *, std::unordered_set<Terminal *> *> firstDict;
    std::unordered_map<Symbol *, std::unordered_set<Terminal *> *> followDict;
};

#endif // !PARSER_H
