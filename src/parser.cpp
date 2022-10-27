#include "parser.hpp"
#include "rule.hpp"
#include "symbol.hpp"
#include <unordered_set>
#include <vector>

// class Parser
// public:
void Parser::parse(std::vector<Terminal *> *input) {}
void Parser::makeTable() {}
void Parser::importFromFile(const char *filename);
void Parser::exportToFile(const char *filename);
void Parser::printFirst();
void Parser::printFollow();

// private:
std::unordered_set<Terminal *> Parser::first(Symbol *s) {
    /**
     *
     * First(s) = {s} if s is terminal.
     *
     * If s is non-terminal,
     * if s -> epsilon, add epsilon to first(s);
     * for S -> s1s2...sn, if s1s2...s{i-1} -*> epsilon, add si to first(S).
     *
     */
    std::unordered_set<Terminal *> firstSet;
    if (s->getType() == SymbolType(terminal)) {
        firstSet.insert((Terminal *)s);
    } else {
        // s is non-terminal
        vector<Rule> atLhsRules = grammar->atLhsRules((Variable *)s);
        for (auto &r : atLhsRules) {
            // Check if r can go to all epsilon
            bool isEpsilon = true;
            for (auto &sym : r.rhs) {
                if (sym->getType() == SymbolType(terminal)) {
                    if (sym->getIdentifier() == grammar->epsilon) {
                    }
                }
            }
        }
    }
}
std::unordered_set<Terminal *> Parser::follow(Symbol *s);
