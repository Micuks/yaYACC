#include "parser.hpp"
#include "rule.hpp"
#include "symbol.hpp"
#include <cassert>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

// class Parser
// public:
void Parser::parse(std::vector<Terminal *> *input) {}

void Parser::dropTable() {
    if (parseTable) {
        for (int i = 0; i < lenParseTable; i++) {
            int *tupleI = parseTable[i];
            if (tupleI) {
                delete[] tupleI;
            }
        }

        delete[] parseTable;
    }
}

void Parser::makeTable() {
    // Drop parseTable if it already exists.
    dropTable();
    // Create new parseTable
    lenParseTable = grammar->variables.size();
    parseTable = new int *[lenParseTable];
    for (int i = 0; i < lenParseTable; i++) {
        parseTable[i] = new int[grammar->terminals.size()];
    }

    for (int i = 0; i < lenParseTable; i++) {
        for (int j = 0; j < grammar->terminals.size(); j++) {
            parseTable[i][j] = -1;
        }
    }

    for (int i = 0; i < grammar->rules.size(); i++) {
        auto &epsilon = grammar->epsilon;
        auto &currRule = grammar->rules[i];

        bool epsExists = false;
        bool bosExists = false;
        Symbol *a = currRule.rhs[0];
        Variable *A = currRule.lhs;

        // for each terminal in FIRST(a), add A->a... to M[A, t].
        auto firstSet = first(a);
        for (auto &t : firstSet) {
            if (t == epsilon) {
                epsExists = true;
            } else {
                parseTable[A->getIndex()][t->getIndex()] = i;
            }
        }

        // If epsilon exists in FIRST(a), add A->a... to M[A, t] for each
        // terminal in FOLLOW(A).
        if (epsExists) {
            auto followSet = follow(A);
            auto &bos = grammar->bos;
            for (auto &t : followSet) {
                if (t == bos) {
                    bosExists = true;
                } else {
                    parseTable[A->getIndex()][bos->getIndex()] = i;
                }
            }

            // Add A->a... to M[A, $] if epsilon in FIRST(a) and $ in FOLLOW(A).
            if (bosExists) {
                parseTable[A->getIndex()][bos->getIndex()] = i;
            }
        }
    }
}

void Parser::printParseTable() {
    // TODO: If this prints incorrect parseTable, change to index by
    // A->getIndex()
    std::cout << "~\t";
    for (auto &a : grammar->terminals) {
        std::cout << a << "\t";
    }
    std::cout << std::endl;
    for (int i = 0; i < lenParseTable; i++) {
        std::cout << "[" << grammar->variables[i] << "]"
                  << ":\t";
        for (int j = 0; j < grammar->terminals.size(); j++) {
            std::cout << parseTable[i][j] << " |\t";
        }
        std::cout << std::endl;
    }
}

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

    // First check if FIRST(s) has already been calculated.
    // If FIRST(s) is found in firstDict, return it directly.
    if (firstDict.find(s) != firstDict.end()) {
#ifdef DEBUG_PARSER
        std::cout << "FIRST(" << *s
                  << ") is alreay in firstDict. Return cached "
                     "followDict: \n";
        for (auto &a : *(firstDict.at(s))) {
            std::cout << *a << " ";
        }
        std::cout << std::endl;
#endif // DEBUG_PARSER
        return *(firstDict.at(s));
    }

    // If not found, calculate it.
#ifdef DEBUG_PARSER
    std::cout << *s << " not found in firstDict. Calculate it.\n";
#endif // DEBUG_PARSER
    std::unordered_set<Terminal *> firstSet;
    auto &epsilon = grammar->epsilon;

#ifdef DEBUG_PARSER_FIRST
    std::cout << "currSymbol: " << *s << std::endl;
#endif // DEBUG_PARSER_FIRST

    if (s->getType() == SymbolType(terminal)) {
        firstSet.insert((Terminal *)s);
    } else {
        // s is non-terminal
#ifdef DEBUG_PARSER_FIRST
        std::cout << *s << " is non-terminal.\n";
#endif // DEBUG_PARSER_FIRST
        vector<Rule> atLhsRules = grammar->atLhsRules((Variable *)s);
        for (auto &r : atLhsRules) {
#ifdef DEBUG_PARSER_FIRST
            std::cout << "currRule: " << r;
#endif // DEBUG_PARSER_FIRST

            // Check if r can go to epsilon
            Terminal *nonEpsilonTerminal = nullptr;
            Variable *nonEpsilonVariable = nullptr;
            for (auto &sym : r.rhs) {
#ifdef DEBUG_PARSER_FIRST
                std::cout << "currSymInRhs: " << *sym
                          << ((sym->getType()) ? " [Terminal]" : " [Variable]")
                          << std::endl;
#endif // DEBUG_PARSER_FIRST
                if (sym->getType() == SymbolType(terminal)) {
                    if (sym == epsilon) {
                        continue;
                    } else {
                        nonEpsilonTerminal = (Terminal *)sym;
                        break;
                    }
                } else if (sym->getType() == SymbolType(variable)) {
                    // First symbol at rule.rhs after epsilon is non-terminal.
                    if (grammar->toEpsilonDirectly((Variable *)sym)) {
#ifdef DEBUG_PARSER_FIRST
                        std::cout << "toEpsilonDirectly" << std::endl;
#endif // DEBUG_PARSER_FIRST
                        continue;
                    } else {
                        // Add its First to FIRST(S).
                        nonEpsilonVariable = (Variable *)sym;
                    }
                }
            }
            if (nonEpsilonTerminal != nullptr) {
                // for S -> s1s2...sn, if s1s2...s_{i-1} -*> epsilon, add si
                // to first(S).
                firstSet.insert(nonEpsilonTerminal);
            } else if (nonEpsilonVariable != nullptr) {
                auto firstSetOfSym = first(nonEpsilonVariable);
                for (auto &a : firstSetOfSym) {
                    firstSet.insert(a);
                }
            } else {
                // If S -*> epsilon, add epsilon to first(S).
                firstSet.insert((Terminal *)epsilon);
            }
        }
    }

    // Put firstSet to firstDict
#ifdef DEBUG_PARSER
    std::cout << "add [ " << *s << ", { ";
    for (auto &a : firstSet) {
        std::cout << *a << ", ";
    }
    std::cout << "} ] to firstDict.\n";
#endif // DEBUG_PARSER
    firstDict[s] = new std::unordered_set<Terminal *>(firstSet);
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
 *
 * TODO: Fix bug of endless recursion appeared in example 4.7 test.
 */
std::unordered_set<Terminal *> Parser::follow(Symbol *s) {

    // First check if FOLLOW(s) has already been calculated.
    // If FOLLOW(s) is found in followDict, return it directly.
    if (followDict.find(s) != followDict.end()) {
#ifdef DEBUG_PARSER
        std::cout << "FOLLOW(" << *s
                  << ") is alreay in followDict. Return cached "
                     "followDict: \n";
        for (auto &a : *(followDict.at(s))) {
            std::cout << *a << " ";
        }
        std::cout << std::endl;
#endif // DEBUG_PARSER
        return *(followDict.at(s));
    }

    // If not found, calculate it.
#ifdef DEBUG_PARSER
    std::cout << *s << " not found in followDict. Calculate it.\n";
#endif // DEBUG_PARSER
    std::unordered_set<Terminal *> followSet;
    if (s->getType() == SymbolType(terminal)) {
        // FOLLOW(s) = BOTTOM_OF_STACK if s is terminal
        followSet.insert(grammar->bos);
    } else if (s->getType() == SymbolType(variable)) {

#ifdef DEBUG_PARSER
        std::cout << "currVariable: " << *s << std::endl;
#endif // DEBUG_PARSER

        auto &epsilon = grammar->epsilon;
        // Add $ to FOLLOW(S) if S is start symbol of grammar.
        if (s == grammar->startSymbol) {
#ifdef DEBUG_PARSER
            std::cout << *s << " is start symbol. Insert [" << *(grammar->bos)
                      << "]" << std::endl;
#endif // DEBUG_PARSER
            followSet.insert(grammar->bos);
        }

        auto atRhsRuleSet = grammar->atRhsRules(s);
        // * for each rule A -> ...S...,
        for (auto &r : atRhsRuleSet) {
#ifdef DEBUG_PARSER
            std::cout << "------------" << std::endl;
            std::cout << "currRule: " << r;
            std::cout << "rule.rhs.size: " << r.rhs.size() << std::endl;
#endif // DEBUG_PARSER
            auto posS = std::find(r.rhs.begin(), r.rhs.end(), s);
#ifdef DEBUG_PARSER
            std::cout << "posS: " << posS - r.rhs.begin() << std::endl;
#endif // DEBUG_PARSER

            // * if A -> aSB, then add FIRST(B) - {EPSILON} to FOLLOW(S).
            if (posS != r.rhs.end() - 1) {
                bool hasEpsilon = true;
#ifdef DEBUG_PARSER
                std::cout << "currVariable is not the last symbol. Add FIRST("
                          << **(posS + 1) << ")." << endl;
#endif // DEBUG_PARSER
                auto nextSymbolPos = posS + 1;
                // If FIRST(current symbol) contains EPSILON,
                //     take next symbol into consideration.
                while (hasEpsilon && nextSymbolPos != r.rhs.end()) {
                    hasEpsilon = false;
#ifdef DEBUG_PARSER
                    std::cout << "sNextSymbol: " << **nextSymbolPos
                              << std::endl;
#endif // DEBUG_PARSER
                    auto sNextSymbolFirstSet = first(*nextSymbolPos);
#ifdef DEBUG_PARSER
                    std::cout << "\nsNextSymbolFirstSet: " << std::endl;
                    for (auto &a : sNextSymbolFirstSet) {
                        std::cout << *a << " ";
                    }
                    std::cout << std::endl;
#endif // DEBUG_PARSER
                    for (auto &a : sNextSymbolFirstSet) {
#ifdef DEBUG_PARSER
                        std::cout << "currSymbolInFirstSet: " << *a
                                  << std::endl;
#endif // DEBUG_PARSER
                        if (a != epsilon) {
                            followSet.insert(a);
                        } else if (a == epsilon) {
                            hasEpsilon = true;
                        }
                    }

                    // Move to next symbol
                    nextSymbolPos++;
                }
                // * if A -> aSB where EPSILON in FIRST(B), then add
                // FOLLOW(A) to FOLLOW(S).
                if (hasEpsilon) {
#ifdef DEBUG_PARSER
                    std::cout << "Add FOLLOW(" << *(r.lhs) << ") to FOLLOW("
                              << *s << ")\n";
#endif // DEBUG_PARSER

                    // S is the same as A. Skip it to avoid endless loop.
                    if (*(r.lhs) == *s) {
#ifdef DEBUG_PARSER
                        std::cout
                            << "rule.lhs is the same as S. Skip it to avoid "
                               "endless loop."
                            << std::endl;
#endif // DEBUG_PARSER
                    } else {

                        auto followSetOfA = toResolveFollow(r.lhs);
                        for (auto &a : followSetOfA) {
                            followSet.insert(a);
                        }
                    }
                }

#ifdef DEBUG_PARSER
                std::cout << "followSet after insert: " << std::endl;
                for (auto &a : followSet) {
                    std::cout << *a << " ";
                }
                std::cout << std::endl;
                std::cout << "--- end of current rule ---" << std::endl;
#endif // DEBUG_PARSER
            } else if (posS == r.rhs.end() - 1) {
                if (*(r.lhs) == *s) {
                    // the last symbol of current rule.rhs is the same as
                    // current rule.lhs. Skip it to avoid endless loop.
#ifdef DEBUG_PARSER
                    std::cout
                        << " the last symbol of current rule.rhs is the same "
                           "as current rule.lhs. Skip it to avoid endless loop."
                        << std::endl;
#endif // DEBUG_PARSER
                } else {

#ifdef DEBUG_PARSER
                    std::cout << "currSymbol is the last symbol. Add FOLLOW("
                              << *(r.lhs) << ")" << std::endl;
#endif // DEBUG_PARSER
       // * if A -> aS then add FOLLOW(A) to FOLLOW(S).
                    auto followSetOfA = toResolveFollow(r.lhs);
                    for (auto &a : followSetOfA) {
                        followSet.insert(a);
                    }
                }
#ifdef DEBUG_PARSER
                std::cout << "followSet after insert: " << std::endl;
                for (auto &a : followSet) {
                    std::cout << *a << " ";
                }
                std::cout << std::endl;
                std::cout << "--- end of current rule ---" << std::endl;
#endif // DEBUG_PARSER
            }
        }
    } else {
        throw std::runtime_error(std::string("ERROR: Unknown Symbol Type"));
    }

    // Resolve unresolved recursive follow() call.
    followSet = resolveFollow(s, followSet);

#ifdef DEBUG_PARSER
    std::cout << "add [ " << *s << ", { ";
    for (auto &a : followSet) {
        std::cout << *a << ", ";
    }
    std::cout << "} ] to followDict.\n";
#endif // DEBUG_PARSER
    followDict[s] = new std::unordered_set<Terminal *>(followSet);
    return followSet;
}

std::unordered_set<Terminal *> Parser::toResolveFollow(Symbol *rSym) {
    return follow(rSym);
}
std::unordered_set<Terminal *> &
Parser::resolveFollow(Symbol *s, std::unordered_set<Terminal *> &followSet) {
    return followSet;
}
