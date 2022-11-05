#ifndef PARSER_H
#define PARSER_H
#include "main.hpp"
#include <functional>
#include <unordered_map>
#include <utility>
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

  protected:
    std::unordered_set<Terminal *> first(Symbol *s);
    std::unordered_set<Terminal *> follow(Symbol *s);

    std::unordered_set<Terminal *> toResolveFollow(Symbol *rSym);
    std::unordered_set<Terminal *> &
    resolveFollow(Symbol *s, std::unordered_set<Terminal *> &followSet);

    std::unordered_map<Symbol *, std::unordered_set<Terminal *> *> firstDict;
    std::unordered_map<Symbol *, std::unordered_set<Terminal *> *> followDict;

    Grammar *grammar;
    bool verbose;

  private:
    std::string stackToString(std::stack<Symbol *> pda);
    std::string remainingTokensToString(std::vector<Terminal *>::iterator it,
                                        std::vector<Terminal *> *tokens);

    int **parseTable;
    int lenParseTable;
};

class LR1Parser : public Parser {
  public:
    void parse(std::vector<Terminal *> *tokens);

    void printLR1ItemSets(std::ostream &os);
    void printLR1ParseTable(std::ostream &os);

    class ItemHasher {
      public:
        size_t operator()(const LR1Item &obj) const {
            static const std::hash<Symbol *> hasher;
            return (hasher(obj.lhs) << 16) + hasher(obj.rhs.front()) +
                   (obj.dotPos << 8);
        }
    };

    using ItemSet = std::unordered_set<LR1Item, ItemHasher>;

    std::vector<ItemSet> itemSets;

  private:
    enum class ActionType { REDUCE, SHIFT_GOTO };
    class Action {
      public:
        // Action type
        ActionType type;
        // Rule to use for reduce/accept
        Rule rule;
        // DFA state index
        int state;
    };

    void getClosure(ItemSet itemSet);
    ItemSet getGo(ItemSet &itemSet, Symbol *symbolToGo);

    int go(ItemSet &itemSet, Symbol *sym);

    class PairHasher {
      public:
        size_t operator()(const std::pair<int, Symbol *> &obj) const {
            // Use hash conflict to aggregate entries with the same state
            return std::hash<int>()(obj.first);
        }
    };

    std::unordered_map<std::pair<int, Symbol *>, Action, PairHasher> parseTable;
};

#endif // !PARSER_H
