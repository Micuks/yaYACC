#include "grammar.hpp"
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <unordered_set>
#include <vector>

vector<Rule> Grammar::atLhsRules(Variable *v) {
    vector<Rule> retVec;
    for (Rule &r : rules) {
        if (*(r.lhs) == *v) {
            retVec.push_back(r);
        } else {
        }
    }
    return retVec;
}

vector<Rule> Grammar::atRhsRules(Symbol *s) {
    vector<Rule> retVec;
    for (Rule r : rules) {
        bool inRRhs = false;
        for (auto &a : r.rhs) {
            if (*a == *s) {
                inRRhs = true;
                break;
            }
        }
        if (inRRhs) {
            // if (s->getType() == SymbolType(variable) && r.lhs != s) {
            //     retVec.push_back(r);
            // }
            retVec.push_back(r);
        }
    }
    return retVec;
}

void Grammar::loadGrammar(const std::string &filename) {
    loadGrammar(filename.c_str());
}

void Grammar::loadGrammar(const char *filename) {
    int cnt = 0;
    Variable *preVar;
    Variable *currVar;
    Rule currRule;

    fstream file;
    file.open(filename, fstream::in);

    if (!file.is_open()) {
        throw std::runtime_error(std::string("ERROR: Failed to open file ") +
                                 std::string(filename));
    }

    // Add bottom of stack symbol to terminals
    terminals.push_back(bos);

    string line;
    while (getline(file, line)) {
        if (line.length() == 0) {
            continue;
        }
        // Initialize current rule
        currRule = Rule();

        istringstream ss(line);
        string token;

        while (ss >> token) {
            if (currRule.lhs == nullptr) {
                // Read token to create lhs of current rule
                bool isExist = false;
                for (auto &a : variables) {
                    if (a->getIdentifier() == token) {
                        currRule.lhs = a;
                        isExist = true;
                        break;
                    }
                }
                if (!isExist) {
                    // Lhs symbol not added to variables vector yet.
                    // Add it.
                    auto sym = new Variable(cnt++, variables.size(), token);
                    variables.push_back(sym);
                    currRule.lhs = sym;
                }
            } else if (token == "->") {
                // Previous symbol is lhs of current rule.
                // eat it!
            } else if (token == "|") {
                // Create new rule with same lhs.
                rules.push_back(currRule);
                currRule = Rule();
                currRule.lhs = rules.back().lhs;
            } else if (token[0] == '<') {
                bool isExist = false;
                for (auto &a : terminals) {
                    if (a->getIdentifier() == token) {
                        currRule.rhs.push_back(a);
                        isExist = true;
                        break;
                    }
                }
                if (!isExist) {
                    auto sym = new Terminal(
                        cnt++, terminals.size(), token,
                        regex(token.substr(1, token.length() - 2)));
                    terminals.push_back(sym);
                    currRule.rhs.push_back(sym);
                }
            } else {
                bool isExist = false;
                if (token == epsilon->getIdentifier()) {
                    currRule.rhs.push_back(epsilon);
                } else {
                    for (auto &a : variables) {
                        if (a->getIdentifier() == token) {
                            currRule.rhs.push_back(a);
                            isExist = true;
                            break;
                        }
                    }
                    if (!isExist) {
                        auto sym = new Variable(cnt++, variables.size(), token);
                        variables.push_back(sym);
                        currRule.rhs.push_back(sym);
                    }
                }
            }
            token = token;
        }
        // End of rule, add it to ruleset
        rules.push_back(currRule);
    }
    // Set start symbol
    startSymbol = variables[0];
    // Remember to convert cfg to ll(1) grammar if the grammar is to be used by
    // ll1 parser.
    file.close();
}

void Grammar::printRules() {
    int cnt = 0;
    cout << "Rules:" << endl;
    for (auto &a : rules) {
        cout << cnt++ << ": ";
        a.printRule();
        cout << endl;
    }
}

Symbol *Grammar::getSymbol(int tag) {
    if (tag == -1) {
        return epsilon;
    }
    for (const auto &a : terminals) {
        if (a->getTag() == tag) {
            return a;
        }
    }
    for (const auto &a : variables) {
        if (a->getTag() == tag) {
            return a;
        }
    }
    return nullptr;
}

Terminal *Grammar::matchTerminal(string str) {
    for (auto &a : terminals) {
        assert(a != nullptr);
        if (a->matcher(str)) {
            return a;
        }
    }
    return nullptr;
}

bool Grammar::toEpsilonDirectly(Variable *sym) {
    auto ruleSet = atLhsRules(sym);
    for (auto &a : ruleSet) {
        if (a.rhs.size() == 1 && a.rhs.at(0) == epsilon) {
            return true;
        }
    }
    return false;
}

// private
void Grammar::cfg2LL1() {
    // First, elimite direct left recursion
    bool isLeftRecursionExists = false;
    std::unordered_set<Variable *> variablesWhoseRulesHaveLeftRecursion;
    vector<Rule> rulesToAdd;
    vector<Rule> rulesToDelete;
    std::unordered_set<Variable *> newlyAddedVariables;
    Variable *newVariable;
    int tagCnt = terminals.size() + variables.size();
    int variablesIndexCnt = variables.size();

    // First, detect if there's any left-recursive rules in the rule set.
    isLeftRecursionExists =
        collectVariablesHaveLeftRecursion(variablesWhoseRulesHaveLeftRecursion);

    if (!isLeftRecursionExists) {
        std::cout << "Given CFG is suitable for LL(1), needless to convert."
                  << std::endl;
    } else {
        // Eliminate left recursion.
        std::cout << "Given CFG have left recursion. Eliminating then..."
                  << std::endl;

        for (auto &v : variablesWhoseRulesHaveLeftRecursion) {

            newVariable = getNewVariable(newlyAddedVariables, v, tagCnt,
                                         variablesIndexCnt);

            auto vRules = atLhsRules(v);
            for (auto &r : vRules) {

                auto &lhs = r.lhs;
                auto &rhs = r.rhs;

                // If rhs[0] == lhs, eliminate left recursion. Else, add new
                // rule with newVariable as lhs.
                if (rhs[0]->getIdentifier() == lhs->getIdentifier()) {

                    vector<Symbol *> newRhs;
                    for (auto it = rhs.begin() + 1; it != rhs.end(); it++) {
                        newRhs.push_back(*it);
                    }

                    Rule newRule = Rule(lhs, newRhs);
                    rulesToAdd.push_back(newRule);
                    rulesToDelete.push_back(r);
                } else {

                    // Add new rule with newVariable as lhs.
                    vector<Symbol *> newRhs;
                    for (auto it = rhs.begin(); it != rhs.end(); it++) {
                        newRhs.push_back(*it);
                    }
                    newRhs.push_back(newVariable);

                    Rule newRule = Rule(lhs, newRhs);
                    rulesToAdd.push_back(newRule);
                    rulesToDelete.push_back(r);
                }
            }

            // add newVariable -> EPSILON
            vector<Symbol *> newRhs;
            newRhs.push_back(epsilon);
            rulesToAdd.push_back(Rule(newVariable, newRhs));
        }

        // Add and delete rules based on rulesToAdd and RulesToDelete.
        for (auto &v : newlyAddedVariables) {
            variables.push_back(v);
        }

        for (auto jt = rulesToDelete.begin(); jt != rulesToDelete.end(); jt++) {
            bool found = false;
            for (auto it = rules.begin(); it != rules.end(); it++) {
                if ((*it) == (*jt)) {

                    rules.erase(it); // If deletes an element, break loop to
                                     // avoid error access.
                    break;
                }
            }
        }
        // Insert newly created rules.
        for (auto &r : rulesToAdd) {
            rules.push_back(r);
        }
    }
}

bool Grammar::collectVariablesHaveLeftRecursion(
    std::unordered_set<Variable *> &variablesWhoseRulesHaveLeftRecursion) {

    bool isLeftRecursionExists = false;
    for (auto &r : rules) {
        auto &lhs = r.lhs;
        auto &rhs = r.rhs;
        if (rhs[0]->getIdentifier() == lhs->getIdentifier()) {

            // Have direct left recursion in current rule
            isLeftRecursionExists = true;
            if (variablesWhoseRulesHaveLeftRecursion.find(lhs) ==
                variablesWhoseRulesHaveLeftRecursion.end()) {
                variablesWhoseRulesHaveLeftRecursion.insert(
                    lhs); // WARNING: There may be variables whose
                          // identifiers are the same but are inserted more
                          // than one time.
            }
        }
    }
    return isLeftRecursionExists;
}

/**
 *
 * Check is new variable used to elimitate left recursion already
 * added. If not, create and add it.
 */
Variable *
Grammar::getNewVariable(std::unordered_set<Variable *> &newlyAddedVariables,
                        Variable *lhs, int &tagCnt, int &variablesIndexCnt) {
    for (auto &a : newlyAddedVariables) {
        if (a->getIdentifier() == lhs->getIdentifier() + "'") {
            return a;
        }
    }

    Variable *newVariable =
        new Variable(tagCnt++, variablesIndexCnt++, lhs->getIdentifier() + "'");
    newlyAddedVariables.insert(newVariable);
    return newVariable;
}

// LR1Grammar

/**
 * Generate augmenteted grammar for LR(1)
 */
void LR1Grammar::augmenting() {

    Variable *start =
        new Variable(terminals.size() + variables.size(), variables.size(),
                     startSymbol->getIdentifier() + "'");
    // Append to variables.
    variables.push_back(start);
    // Set as new startSymbol.
    auto oldStartSymbol = startSymbol;
    startSymbol = start;

    // Add new rule S' -> S.
    Rule newRule(startSymbol, std::vector<Symbol *>{oldStartSymbol});
    rules.insert(rules.begin(), newRule);
    // rules.push_back(newRule);
}

Grammar *LR1Grammar::getBase() {
    Grammar *g = new Grammar();

    g->terminals = terminals;
    g->variables = variables;
    g->rules = rules;
    g->startSymbol = startSymbol;
    g->epsilon = epsilon;
    g->bos = bos;

    return g;
}
