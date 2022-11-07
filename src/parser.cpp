#include "parser.hpp"
#include "rule.hpp"
#include "symbol.hpp"
#include <cassert>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

// class Parser
// public:
void Parser::parse(std::vector<Terminal *> *tokens) {
    std::stack<Symbol *> pda = std::stack<Symbol *>();
    const Grammar *g = grammar;
    assert(parseTable != nullptr);

    int currPos = 0;
    std::vector<Terminal *>::iterator it = (*tokens).begin();

    pda.push(g->bos);
    pda.push(g->startSymbol);

    while (pda.size()) {
        const Terminal *token = *it;
        Symbol *tos = pda.top(); // Top of stack

#ifdef DEBUG_PARSER
        std::cout << "Current token: " << **it
                  << ", current pos: " << it - tokens->begin() << std::endl;
#endif // DEBUG_PARSER
       // Try to match curent terminal in token with symbol at the top of
       // stack.
        if (token->getTag() == (tos->getTag())) {

            // tos matches token
            if (verbose) {
                std::cout << "Symbol matched: " << tos->getIdentifier()
                          << std::endl;
            }
            it++;
            currPos++;
            pda.pop();
        } else {
            // Tos failed to match, push symbols onto stack.
            if (tos->getType() == Symbol::SymbolType::terminal) {

                // Error occurred, unmatch terminal on top of stack
                throw std::runtime_error(
                    std::string("ERROR: Unknown symbol at position ") +
                    std::to_string(it - tokens->begin()) +
                    std::string(". REJECT INPUT STRING.\n") +
                    stackToString(pda) + remainingTokensToString(it, tokens));
            } else {

                // Get parseTable[variable][terminal].
                int ruleIdx = parseTable[tos->getIndex()][token->getIndex()];
                if (ruleIdx == -1) {
                    stringstream ss;
                    ss << "Current elements in stack (bottom to top):\n";
                    // Print stack for debug.
                    std::stack<Symbol *> rvsPda;
                    while (!pda.empty()) {
                        rvsPda.push(pda.top());
                        pda.pop();
                    }
                    while (!rvsPda.empty()) {
                        ss << *(rvsPda.top()) << ", ";
                        rvsPda.pop();
                    }
                    ss << std::endl;

                    ss << "Remaining unparsed input string:\n";
                    // Print remained tokens for debug.
                    for (auto a = it; a != tokens->end(); a++) {
                        ss << **a << ", ";
                    }
                    ss << std::endl;

                    throw std::runtime_error(
                        std::string("ERROR: No entry of rule at position ") +
                        std::to_string(it - tokens->begin()) +
                        std::string(". REJECT INPUT STRING.\n") + ss.str());
                } else {

                    if (verbose) {
                        std::cout << "rule: " << g->rules[ruleIdx] << std::endl;
                    }
                }

                Rule r = g->rules[ruleIdx];
                pda.pop();
                if (!r.leadToEpsilon(g->epsilon)) {
                    for (std::vector<Symbol *>::reverse_iterator ri =
                             r.rhs.rbegin();
                         ri != r.rhs.rend(); ri++) {
                        pda.push(*ri);
                    }
                }
            }
        }
    }
    if (verbose) {
        std::cout << std::endl;
    }
    std::cout << "INPUT ACCEPTED" << std::endl;
}

void Parser::dropTable() {
    if (parseTable) {
#ifdef DEBUG_PARSER
        std::cout << "ParseTable != nullptr. Drop table. lenParseTable: "
                  << lenParseTable << "\n";
#endif // DEBUG_PARSER
        for (int i = 0; i < lenParseTable; i++) {
            int *tupleI = parseTable[i];
            if (tupleI) {
                delete[] tupleI;
            }
        }

        if (lenParseTable)
            delete[] parseTable;
    }
}

void Parser::makeTable() {
    // Drop parseTable if it already exists.
    dropTable();
    // Create new parseTable
    lenParseTable = grammar->variables.size();
#ifdef DEBUG_PARSER
    std::cout << "lenParseTable: " << lenParseTable << std::endl;
#endif // DEBUG_PARSER
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
                    parseTable[A->getIndex()][t->getIndex()] = i;
                }
            }

            // Add A->a... to M[A, $] if EPSILON in FIRST(a) and $ in FOLLOW(A).
            if (bosExists) {
#ifdef DEBUG_PARSER
                std::cout << "EPSILON in FIRST(a) and $ in FOLLOW(A). Add "
                          << currRule << " to parseTable[" << A->getIndex()
                          << "][" << bos->getIndex() << "]\n";
#endif // DEBUG_PARSER
                parseTable[A->getIndex()][bos->getIndex()] = i;
            }
        }
    }
}

std::string Parser::parseTableToString() {
    stringstream ss;
    // Hint: If this prints incorrect parseTable, change to index by
    // A->getIndex()
    const int fixedWidth = 17;
    const char separator = ' ';
    ss << std::left << std::setw(fixedWidth) << std::setfill(separator)
       << "VARIABLE";
    for (auto &a : grammar->terminals) {
        ss << std::left << std::setw(fixedWidth) << std::setfill(separator)
           << *(a);
    }
    ss << std::endl;

    for (int i = 0; i < lenParseTable; i++) {
        // assert(grammar->variables[i] != nullptr);
        ss << std::left << std::setw(fixedWidth) << std::setfill(separator)
           << *(grammar->variables[i]);

        for (int j = 0; j < grammar->terminals.size(); j++) {
            int ruleIdx = parseTable[i][j];

            if (ruleIdx == -1) {
                ss << std::setw(fixedWidth) << std::left
                   << std::setfill(separator) << "error";
            } else {
                ss << std::setw(fixedWidth) << std::left
                   << std::setfill(separator) << grammar->rules[ruleIdx];
            }
        }
        ss << std::endl;
    }
    return ss.str();
}

void Parser::printParseTable() { std::cout << parseTableToString(); }

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

/**
 *
 * First(s) = {s} if s is terminal.
 *
 * If s is non-terminal,
 * if s -> epsilon, add epsilon to first(s);
 * for S -> s1s2...sn, if s1s2...s{i-1} -*> epsilon, add si to first(S).
 *
 */
std::unordered_set<Terminal *> Parser::first(Symbol *s) {

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

    if (s->getType() == Symbol::SymbolType::terminal) {
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
                if (sym->getType() == Symbol::SymbolType::terminal) {
                    if (sym == epsilon) {
                        continue;
                    } else {
                        nonEpsilonTerminal = (Terminal *)sym;
                        break;
                    }
                } else if (sym->getType() == Symbol::SymbolType::variable) {
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
#ifdef DEBUG_PARSER_FOLLOW
        std::cout << "FOLLOW(" << *s
                  << ") is alreay in followDict. Return cached "
                     "followDict: \n";
        for (auto &a : *(followDict.at(s))) {
            std::cout << *a << " ";
        }
        std::cout << std::endl;
#endif // DEBUG_PARSER_FOLLOW
        return *(followDict.at(s));
    }

    // If not found, calculate it.
#ifdef DEBUG_PARSER_FOLLOW
    std::cout << *s << " not found in followDict. Calculate it.\n";
#endif // DEBUG_PARSER_FOLLOW
    std::unordered_set<Terminal *> followSet;
    if (s->getType() == Symbol::SymbolType::terminal) {
        // FOLLOW(s) = BOTTOM_OF_STACK if s is terminal
        followSet.insert(grammar->bos);
    } else if (s->getType() == Symbol::SymbolType::variable) {

#ifdef DEBUG_PARSER_FOLLOW
        std::cout << "currVariable: " << *s << std::endl;
#endif // DEBUG_PARSER_FOLLOW

        auto &epsilon = grammar->epsilon;
        // Add $ to FOLLOW(S) if S is start symbol of grammar.
        if (s == grammar->startSymbol) {
#ifdef DEBUG_PARSER_FOLLOW
            std::cout << *s << " is start symbol. Insert [" << *(grammar->bos)
                      << "]" << std::endl;
#endif // DEBUG_PARSER_FOLLOW
            followSet.insert(grammar->bos);
        }

        auto atRhsRuleSet = grammar->atRhsRules(s);
        // * for each rule A -> ...S...,
        for (auto &r : atRhsRuleSet) {
#ifdef DEBUG_PARSER_FOLLOW
            std::cout << "------------" << std::endl;
            std::cout << "currRule: " << r;
            std::cout << "rule.rhs.size: " << r.rhs.size() << std::endl;
#endif // DEBUG_PARSER_FOLLOW
            auto posS = std::find(r.rhs.begin(), r.rhs.end(), s);
#ifdef DEBUG_PARSER_FOLLOW
            std::cout << "posS: " << posS - r.rhs.begin() << std::endl;
#endif // DEBUG_PARSER_FOLLOW

            // * if A -> aSB, then add FIRST(B) - {EPSILON} to FOLLOW(S).
            if (posS != r.rhs.end() - 1) {
                bool hasEpsilon = true;
#ifdef DEBUG_PARSER_FOLLOW
                std::cout << "currVariable is not the last symbol. Add FIRST("
                          << **(posS + 1) << ")." << endl;
#endif // DEBUG_PARSER_FOLLOW
                auto nextSymbolPos = posS + 1;
                // If FIRST(current symbol) contains EPSILON,
                //     take next symbol into consideration.
                while (hasEpsilon && nextSymbolPos != r.rhs.end()) {
                    hasEpsilon = false;
#ifdef DEBUG_PARSER_FOLLOW
                    std::cout << "sNextSymbol: " << **nextSymbolPos
                              << std::endl;
#endif // DEBUG_PARSER_FOLLOW
                    auto sNextSymbolFirstSet = first(*nextSymbolPos);
#ifdef DEBUG_PARSER_FOLLOW
                    std::cout << "\nsNextSymbolFirstSet: " << std::endl;
                    for (auto &a : sNextSymbolFirstSet) {
                        std::cout << *a << " ";
                    }
                    std::cout << std::endl;
#endif // DEBUG_PARSER_FOLLOW
                    for (auto &a : sNextSymbolFirstSet) {
#ifdef DEBUG_PARSER_FOLLOW
                        std::cout << "currSymbolInFirstSet: " << *a
                                  << std::endl;
#endif // DEBUG_PARSER_FOLLOW
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
#ifdef DEBUG_PARSER_FOLLOW
                    std::cout << "Add FOLLOW(" << *(r.lhs) << ") to FOLLOW("
                              << *s << ")\n";
#endif // DEBUG_PARSER_FOLLOW

                    // S is the same as A. Skip it to avoid endless loop.
                    if (*(r.lhs) == *s) {
#ifdef DEBUG_PARSER_FOLLOW
                        std::cout
                            << "rule.lhs is the same as S. Skip it to avoid "
                               "endless loop."
                            << std::endl;
#endif // DEBUG_PARSER_FOLLOW
                    } else {

                        auto followSetOfA = toResolveFollow(r.lhs);
                        for (auto &a : followSetOfA) {
                            followSet.insert(a);
                        }
                    }
                }

#ifdef DEBUG_PARSER_FOLLOW
                std::cout << "followSet after insert: " << std::endl;
                for (auto &a : followSet) {
                    std::cout << *a << " ";
                }
                std::cout << std::endl;
                std::cout << "--- end of current rule ---" << std::endl;
#endif // DEBUG_PARSER_FOLLOW
            } else if (posS == r.rhs.end() - 1) {
                if (*(r.lhs) == *s) {
                    // the last symbol of current rule.rhs is the same as
                    // current rule.lhs. Skip it to avoid endless loop.
#ifdef DEBUG_PARSER_FOLLOW
                    std::cout
                        << " the last symbol of current rule.rhs is the same "
                           "as current rule.lhs. Skip it to avoid endless loop."
                        << std::endl;
#endif // DEBUG_PARSER_FOLLOW
                } else {

#ifdef DEBUG_PARSER_FOLLOW
                    std::cout << "currSymbol is the last symbol. Add FOLLOW("
                              << *(r.lhs) << ")" << std::endl;
#endif // DEBUG_PARSER_FOLLOW
       // * if A -> aS then add FOLLOW(A) to FOLLOW(S).
                    auto followSetOfA = toResolveFollow(r.lhs);
                    for (auto &a : followSetOfA) {
                        followSet.insert(a);
                    }
                }
#ifdef DEBUG_PARSER_FOLLOW
                std::cout << "followSet after insert: " << std::endl;
                for (auto &a : followSet) {
                    std::cout << *a << " ";
                }
                std::cout << std::endl;
                std::cout << "--- end of current rule ---" << std::endl;
#endif // DEBUG_PARSER_FOLLOW
            }
        }
    } else {
        throw std::runtime_error(std::string("ERROR: Unknown Symbol Type"));
    }

    // Resolve unresolved recursive follow() call.
    followSet = resolveFollow(s, followSet);

#ifdef DEBUG_PARSER_FOLLOW
    std::cout << "add [ " << *s << ", { ";
    for (auto &a : followSet) {
        std::cout << *a << ", ";
    }
    std::cout << "} ] to followDict.\n";
#endif // DEBUG_PARSER_FOLLOW
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

std::string Parser::stackToString(std::stack<Symbol *> pda) {
    stringstream ss;
    ss << "Current elements in stack (bottom to top):\n";
    // Print stack for debug.
    std::stack<Symbol *> rvsPda;
    while (!pda.empty()) {
        rvsPda.push(pda.top());
        pda.pop();
    }
    while (!rvsPda.empty()) {
        ss << *(rvsPda.top()) << ", ";
        rvsPda.pop();
    }
    ss << std::endl;
    return ss.str();
}

std::string
Parser::remainingTokensToString(std::vector<Terminal *>::iterator it,
                                std::vector<Terminal *> *tokens) {
    stringstream ss;
    ss << "Remaining unparsed input string:\n";
    // Print remained tokens for debug.
    for (auto a = it; a != tokens->end(); a++) {
        ss << **a << ", ";
    }
    ss << std::endl;
    return ss.str();
}

std::string LR1Parser::lR1ItemSetToString(const ItemSet &itemSet) {
    stringstream ss;
    printItemSet(itemSet, ss);
    return ss.str();
}

// LR1Parser
void LR1Parser::getClosure(ItemSet &itemSet) {
    bool isUpdated = true;

    // Repeat until no new item is added.
    while (isUpdated) {
        isUpdated = false;

        auto isT = [](Symbol *sym) {
            return sym->getType() == Symbol::SymbolType::terminal;
        };

        for (const auto &item : itemSet) {
            // Reduction item
            if (item.dotPos == item.rhs.size()) {
                continue;
            }

            const auto &leader = item.rhs[item.dotPos];

            // Shift item
            if (isT(leader)) {
                continue;
            }

            // Reduction expecting item
            std::unordered_set<Terminal *> lookAheads;
            if (item.dotPos != item.rhs.size() - 1) {
                // Leader is not the last symbol in the rhs.
                for (const auto &lookAhead : first(item.rhs[item.dotPos + 1])) {
                    lookAheads.insert(lookAhead);
                }
            } else {
                // Leader is the last symbol, use the same lookAhead.
                lookAheads.insert(item.lookAhead);
            }

            for (auto &r : grammar->atLhsRules((Variable *)leader)) {
                for (const auto &lookAhead : lookAheads) {
                    LR1Item newItem(r, 0, lookAhead);

                    // if (itemSet.find(newItem) == itemSet.end())
                    if (std::find(itemSet.begin(), itemSet.end(), newItem) ==
                        itemSet.end()) {
                        // WARNING: Not sure whether this find works fine.
                        itemSet.insert(newItem);
                        isUpdated = true;
                    }
                }
            }
        }
    }
}

LR1Parser::ItemSet LR1Parser::getGo(ItemSet &itemSet, Symbol *symbolToGo) {
    ItemSet newItemSet;
    for (auto &item : itemSet) {
        // Reduction item
        if (item.dotPos == item.rhs.size()) {
            continue;
        }

        auto &leader = item.rhs[item.dotPos];
        // If leader is symbolToGo, insert it to newItemSet
        if (*leader == *symbolToGo) {
            LR1Item newItem(item);
            newItem.dotPos++;
            newItemSet.insert(newItem);
        }
    }
    // Get closure of newItemSet
    getClosure(newItemSet);

    return newItemSet;
}

int LR1Parser::go(ItemSet &itemSet, Symbol *sym) {
    auto newItemSet = getGo(itemSet, sym);

    if (newItemSet.empty()) {
        return -1;
    }

    int i = 0;
    for (; i < itemSets.size(); i++) {
        const auto &preItemSet = itemSets[i];
        if (preItemSet == newItemSet) {
            break;
        }
    }

    // go(I, X) is not in itemSets
    if (i == itemSets.size()) {
        itemSets.push_back(std::move(newItemSet));
    }

    // return index of newItemSet
    return i;
}
void LR1Parser::printItemSet(const ItemSet &itemSet, std::ostream &os) {
    for (const auto &item : itemSet) {
        os << "\t" << item << std::endl;
    }
}

void LR1Parser::makeTable() {
    // Add starting item to initialItemSet
    auto initItemSet =
        ItemSet{LR1Item(grammar->atLhsRules(grammar->startSymbol).front(), 0,
                        grammar->epsilon)};

#ifdef DEBUG_LR1PARSER

    std::cout << "\nBefore getClosure:\n";
    printItemSet(initItemSet, cout);
#endif // DEBUG_LR1PARSER

    // Get closure of initItemSet
    getClosure(initItemSet);
#ifdef DEBUG_LR1PARSER

    std::cout << "\nAfter getClosure:\n";
    printItemSet(initItemSet, cout);
#endif // DEBUG_LR1PARSER

    itemSets.push_back(initItemSet);

    Rule emptyRule;

    // Loop until no change
    for (int index = 0; index < itemSets.size(); index++) {

        // Add shift/goto to parseTable
        for (const auto &var : grammar->variables) {
            auto goTo = go(itemSets[index], var);

            if (goTo > 0) {
                parseTable.emplace(
                    std::make_pair(index, var),
                    Action{ActionType::SHIFT_GOTO, emptyRule, goTo});
            }
        }

        // Add shift/goto to parseTable
        for (const auto &ter : grammar->terminals) {
            auto goTo = go(itemSets[index], ter);

            if (goTo > 0) {
                parseTable.emplace(
                    std::make_pair(index, ter),
                    Action{ActionType::SHIFT_GOTO, emptyRule, goTo});
            }
        }

        // Add reduce to parseTable
        for (LR1Item item : itemSets[index]) {
            if (item.dotPos == item.rhs.size()) {
                parseTable.emplace(
                    std::make_pair(index, item.lookAhead),
                    Action{ActionType::REDUCE, item.getBase(), 0});
            }
        }
    }
}

void LR1Parser::printLR1ItemSets(std::ostream &os) {
    int index = 0;
    os << "\nLR1 Item Sets:\n";
    for (const auto &itemSet : itemSets) {
        os << "Item set " << index++ << ":\n";
        printItemSet(itemSet, os);
    }
}

void LR1Parser::printLR1ParseTable(std::ostream &os) {
    os << "\nLR1 Parse Table:\n";
    for (const auto &entry : parseTable) {
        os << "<" << std::setw(2) << std::right << entry.first.first << ", "
           << std::setw(9) << std::left << *(entry.first.second) << "> | ";
        const auto &action = entry.second;

        switch (action.type) {
        case LR1Parser::ActionType::REDUCE:
            if (action.rule.lhs == grammar->startSymbol) {
                os << "ACC";
            } else {
                os << "Reduced by " << action.rule;
            }

            break;
        case LR1Parser::ActionType::SHIFT_GOTO:
            auto &symbol = entry.first.second;
            if (symbol->getType() == Symbol::SymbolType::variable) {
                os << "Goto " << action.state;
            } else if (symbol->getType() == Symbol::SymbolType::terminal) {
                os << "Shift " << action.state;
            }

            break;
        }

        os << std::endl;
    }
}

void LR1Parser::parse(std::vector<Terminal *> *tokens) {
    std::vector<int> stateStack;
    std::vector<Symbol *> symbolStack;

    // Init stateStack
    stateStack.push_back(0);

    // Verbose output
    std::vector<std::tuple<std::string, std::string, std::string>> output;

    int width1 = 5;
    int width2 = 6;
    int width3 = 6;

    std::stringstream stackSs;
    std::stringstream inputSs;
    std::stringstream actionSs;

    try {
        for (auto pToken = tokens->begin(); pToken != tokens->end(); pToken++) {
            auto state = stateStack.back();
            const auto &token = *pToken;

            // Stack
            stackSs << stateStack.front() << " ";
            for (int i = 0; i < symbolStack.size(); i++) {
                stackSs << stateStack[i] << " ";
                stackSs << stateStack[i + 1] << " ";
            }
            if (width1 < stackSs.str().size()) {
                width1 = stackSs.str().size();
            }

            // Tokens
            inputSs.clear();
            for (auto p = pToken; p != tokens->end(); p++) {
                inputSs << **p;
            }
            if (width2 < inputSs.str().size()) {
                width2 = inputSs.str().size();
            }

            // Action
            LR1Parser::Action action;
            try {
                action = parseTable.at(std::make_pair(state, token));
                // WARNING: Not sure whether token stored in Terminal * type can
                // be accessed via this way.

            } catch (...) {

                stringstream ss;
                ss << "No action for (" << state << ", " << *token << ")";
                throw std::runtime_error(ss.str());
            }

            switch (action.type) {
            case LR1Parser::ActionType::SHIFT_GOTO:
                // Output
                actionSs.clear();
                actionSs << "Shift " << action.state;

                // Action
                stateStack.push_back(action.state);
                symbolStack.push_back(token);
                pToken++;
                break;

            case LR1Parser::ActionType::REDUCE:
                if (action.rule.lhs == grammar->startSymbol) {
                    // Accept input, exit parsing...

                    // Output
                    actionSs.clear();
                    actionSs << "ACCEPT";

                    // Action
                    pToken = tokens->end();
                } else {
                    auto lhsReducedTo = action.rule.lhs;

                    // Output
                    actionSs.clear();
                    actionSs << "Reduced by " << action.rule;

                    // Action: reduce k symbols and their states
                    for (auto &a : action.rule.rhs) {
                        symbolStack.pop_back();
                        stateStack.pop_back();
                    }

                    try {
                        action = parseTable.at(
                            std::make_pair(stateStack.back(), lhsReducedTo));
                    } catch (...) {
                        stringstream ss;
                        ss << "No GOTO for (" << stateStack.back() << ", "
                           << lhsReducedTo << ")";
                        throw std::runtime_error(ss.str());
                    }

                    if (action.type != LR1Parser::ActionType::SHIFT_GOTO) {
                        stringstream ss;
                        ss << "Bad GOTO for (" << stateStack.back() << ", "
                           << lhsReducedTo << ")";
                    }

                    // action.type is SHIFT_GOTO
                    stateStack.push_back(action.state);
                    symbolStack.push_back(lhsReducedTo);
                }
                break;
            default:
                throw std::runtime_error(
                    "ERROR: Unknown action type! (I didn't define another "
                    "action type in my memory :-/)");
                break;
            }

            // Output
            output.emplace_back(stackSs.str(), inputSs.str(), actionSs.str());
        }
    } catch (const std::exception &e) {
        actionSs.clear();
        actionSs << e.what();

        output.emplace_back(stackSs.str(), inputSs.str(), actionSs.str());
        if (width3 < actionSs.str().size()) {
            width3 = actionSs.str().size();
        }
    }
}
