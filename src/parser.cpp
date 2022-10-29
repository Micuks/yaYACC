#include "parser.hpp"
#include "rule.hpp"
#include "symbol.hpp"
#include <cassert>
#include <iostream>
#include <unordered_set>
#include <vector>

// class Parser
// public:
void Parser::parse(std::vector<Terminal *> *input) {}

void Parser::makeTable() {}

void Parser::importFromFile(const char *filename) {
    // TODO: Implement it
    return;
}

void Parser::exportToFile(const char *filename) {
    // TODO: Implement it
    return;
}

void Parser::printFirstTable() {
    assert(grammar != nullptr);

    std::cout << "FIRST table:\n";
    auto &variables = grammar->variables;

    for (auto &var : variables) {
        std::cout << *var << ": ";
        auto firstSet = first(var);
        for (auto &ter : firstSet) {
            std::cout << *ter << " ";
        }
        std::cout << std::endl;
    }
}

void Parser::printFollowTable() {
    assert(grammar != nullptr);

    std::cout << "FOLLOW table:\n";
    auto &variables = grammar->variables;

    for (auto &var : variables) {
        std::cout << *var << ": ";
        auto followSet = follow(var);
        for (auto &ter : followSet) {
            std::cout << *ter << " ";
        }
        std::cout << std::endl;
    }
}

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
    auto &epsilon = grammar->epsilon;

#ifdef DEBUG_PARSER
    std::cout << "s: " << *s << std::endl;
#endif // DEBUG_PARSER

    if (s->getType() == SymbolType(terminal)) {
        firstSet.insert((Terminal *)s);
    } else {
        // s is non-terminal
        vector<Rule> atLhsRules = grammar->atLhsRules((Variable *)s);
        for (auto &r : atLhsRules) {
#ifdef DEBUG_PARSER
            std::cout << "currRule: " << r;
#endif // DEBUG_PARSER

            // Check if r can go to epsilon
            Terminal *nonEpsilonTerminal = nullptr;
            for (auto &sym : r.rhs) {
#ifdef DEBUG_PARSER
                std::cout << "currSym: " << *sym
                          << ((sym->getType()) ? " [Terminal]" : " [Variable]")
                          << std::endl;
#endif // DEBUG_PARSER
                if (sym->getType() == SymbolType(terminal)) {
                    if (sym == epsilon) {
                        continue;
                    } else {
                        if (grammar->toEpsilonDirectly((Variable *)sym)) {
#ifdef DEBUG_PARSER
                            std::cout << "toEpsilonDirectly" << std::endl;
#endif // DEBUG_PARSER
                            continue;
                        } else {
                            nonEpsilonTerminal = (Terminal *)sym;
                            break;
                        }
                    }
                }
            }
            if (nonEpsilonTerminal != nullptr) {
                // for S -> s1s2...sn, if s1s2...s_{i-1} -*> epsilon, add si
                // to first(S).
                firstSet.insert(nonEpsilonTerminal);
            } else {
                // If S -*> epsilon, add epsilon to first(S).
                firstSet.insert((Terminal *)epsilon);
            }
        }
    }

    return firstSet;
}

/**
 *
 * Build FOLLOW(S) for symbol S.
 *
 * Add $ to FOLLOW(S) if S is start symbol of grammar;
 *
 * for each rule A -> ...S...,
 * if A -> aSB, then add FIRST(B) - {EPSILON} to FOLLOW(S).
 *
 * if A -> aSB where EPSILON in FIRST(B), then add FOLLOW(A) to FOLLOW(S).
 *
 * if A -> aS then add FOLLOW(A) to FOLLOW(S).
 */
std::unordered_set<Terminal *> Parser::follow(Symbol *s) {
    std::unordered_set<Terminal *> followSet;
    if (s->getType() == SymbolType(terminal)) {
        // FOLLOW(s) = BOTTOM_OF_STACK if s is terminal
        followSet.insert(grammar->bos);
    } else {

        auto &epsilon = grammar->epsilon;
        // Add $ to FOLLOW(S) if S is start symbol of grammar.
        if (s == grammar->startSymbol) {
            followSet.insert(grammar->bos);
        }

        auto atRhsRuleSet = grammar->atRhsRules(s);
        // * for each rule A -> ...S...,
        for (auto &r : atRhsRuleSet) {
            auto posS = std::find(r.rhs.begin(), r.rhs.end(), s);
            bool hasEpsilon = true;
            // * if A -> aSB, then add FIRST(B) - {EPSILON} to FOLLOW(S).
            if (posS != r.rhs.end() - 1) {
                auto sNextSymbolFirstSet = first(*(posS + 1));
                for (auto &a : sNextSymbolFirstSet) {
                    if (a != epsilon) {
                        followSet.insert(a);
                        hasEpsilon = false;
                    }
                }

                // * if A -> aSB where EPSILON in FIRST(B), then add FOLLOW(A)
                // to FOLLOW(S).
                if (hasEpsilon) {
                    auto followSetOfA = follow(r.lhs);
                    for (auto &a : followSetOfA) {
                        followSet.insert(a);
                    }
                }
            } else {
                // * if A -> aS then add FOLLOW(A) to FOLLOW(S).
                auto followSetOfA = follow(r.lhs);
                for (auto &a : followSetOfA) {
                    followSet.insert(a);
                }
            }
        }
    }

    return followSet;
}
