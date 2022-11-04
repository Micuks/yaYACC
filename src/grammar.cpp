#include "grammar.hpp"
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <unordered_set>

vector<Rule> Grammar::atLhsRules(Variable *v) {
    vector<Rule> retVec;
    for (Rule &r : rules) {
#ifdef DEBUG_GRAMMAR
        cout << "r: " << r;
        cout << "r.lhs: " << r.lhs->toString() << ", v: " << v->toString()
             << endl;
#endif // DEBUG_GRAMMAR
        if (*(r.lhs) == *v) {
#ifdef DEBUG_GRAMMAR
            cout << "equal" << endl;
#endif // DEBUG_GRAMMAR
            retVec.push_back(r);
        } else {
#ifdef DEBUG_GRAMMAR
            cout << "neq" << endl;
#endif // DEBUG_GRAMMAR
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
#ifdef DEBUG_GRAMMAR
        cout << "s: " << (*s).toString() << ", "
             << "r: " << r << "inRRhs: " << ((inRRhs) ? "1" : "0") << endl;
#endif // DEBUG_GRAMMAR
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
#ifdef DEBUG
        cout << "New line: " << line << endl;
#endif // DEBUG
       // Initialize current rule
        currRule = Rule();

        istringstream ss(line);
        string token;

        while (ss >> token) {
#ifdef DEBUG
            cout << "currToken[" << token << "]\n";
#endif // DEBUG
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
#ifdef DEBUG_LEX
                    std::cout << token << ".regexPattern="
                              << token.substr(1, token.length() - 2)
                              << std::endl;
#endif
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
    // Convert cfg to ll(1) grammar
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
#ifdef DEBUG_GRAMMAR
    cout << terminals.size();
    for (auto &a : terminals) {
        cout << a->toString() << " ";
    }
#endif // DEBUG_GRAMMAR
    for (auto &a : terminals) {
        assert(a != nullptr);
#ifdef DEBUG_GRAMMAR
        cout << a->getIdentifier() << " ";
#endif // DEBUG_GRAMMAR
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
void Grammar::cfg2ll1() {
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
#ifdef DEBUG_LEFT_RECURSION
            std::cout << "Current variable: " << v->getIdentifier()
                      << std::endl;
#endif // DEBUG_LEFT_RECURSION

            newVariable = getNewVariable(newlyAddedVariables, v, tagCnt,
                                         variablesIndexCnt);
#ifdef DEBUG_LEFT_RECURSION
            std::cout << "New variable: " << newVariable->toString()
                      << std::endl;
#endif // DEBUG_LEFT_RECURSION

            auto vRules = atLhsRules(v);
            for (auto &r : vRules) {
#ifdef DEBUG_LEFT_RECURSION
                std::cout << "Current rule to process: " << r << std::endl;
#endif // DEBUG_LEFT_RECURSION

                auto &lhs = r.lhs;
                auto &rhs = r.rhs;

                // If rhs[0] == lhs, eliminate left recursion. Else, add new
                // rule with newVariable as lhs.
                if (rhs[0]->getIdentifier() == lhs->getIdentifier()) {

#ifdef DEBUG_LEFT_RECURSION
                    std::cout << "Lhs == rhs[0], eliminate recursion."
                              << std::endl;
#endif // DEBUG_LEFT_RECURSION
                    vector<Symbol *> newRhs;
                    for (auto it = rhs.begin() + 1; it != rhs.end(); it++) {
                        newRhs.push_back(*it);
                    }

                    Rule newRule = Rule(lhs, newRhs);
#ifdef DEBUG_LEFT_RECURSION
                    std::cout << "New rule: " << newRule << std::endl;
#endif // DEBUG_LEFT_RECURSION
                    rulesToAdd.push_back(newRule);
                    rulesToDelete.push_back(r);
                } else {

#ifdef DEBUG_LEFT_RECURSION
                    std::cout << "Lhs is not rhs[0], add new rule with "
                                 "newVariable as lhs symbol."
                              << std::endl;
#endif // DEBUG_LEFT_RECURSION
       // Add new rule with newVariable as lhs.
                    vector<Symbol *> newRhs;
                    for (auto it = rhs.begin() + 1; it != rhs.end(); it++) {
                        newRhs.push_back(*it);
                    }
                    newRhs.push_back(newVariable);

                    Rule newRule = Rule(lhs, newRhs);
#ifdef DEBUG_LEFT_RECURSION
                    std::cout << "New rule: " << newRule << std::endl;
#endif // DEBUG_LEFT_RECURSION
                    rulesToAdd.push_back(newRule);
                    rulesToDelete.push_back(r);
                }
            }

            // add newVariable -> EPSILON
            vector<Symbol *> newRhs;
            newRhs.push_back(epsilon);
            rulesToAdd.push_back(Rule(newVariable, newRhs));
        }

#ifdef DEBUG_LEFT_RECURSION
        std::cout << "Variables before add and delete: " << std::endl;
        for (auto &a : variables) {
            std::cout << *a << ", ";
        }
        std::cout << std::endl;
#endif // DEBUG_LEFT_RECURSION

        // Add and delete rules based on rulesToAdd and RulesToDelete.
        for (auto &v : newlyAddedVariables) {
            variables.push_back(v);
        }

#ifdef DEBUG_LEFT_RECURSION
        std::cout << "Variables after add and delete: " << std::endl;
        for (auto &a : variables) {
            std::cout << *a << ", ";
        }
        std::cout << std::endl;
#endif // DEBUG_LEFT_RECURSION

#ifdef DEBUG_LEFT_RECURSION
        std::cout << "Rules before add and delete: " << std::endl;
        printRules();
#endif // DEBUG_LEFT_RECURSION
#ifdef DEBUG_LEFT_RECURSION
        std::cout << "Rules to delete:" << std::endl;
        for (auto &r : rulesToDelete) {
            std::cout << r << std::endl;
        }
        std::cout << "Rules to add:" << std::endl;
        for (auto &r : rulesToAdd) {
            std::cout << r << std::endl;
        }
        std::cout << std::endl;
#endif // DEBUG_LEFT_RECURSION

        for (auto it = rules.begin(); it != rules.end(); it++) {
            bool found = false;
            for (auto jt = rulesToDelete.begin(); jt != rulesToDelete.end();
                 jt++) {
                if ((*it) == (*jt)) {
                    found = true;
                    break;
                }
            }
            if (found) {
                it = rules.erase(it); // If deletes an element, it points to
                                      // next element following the element
                                      // deleted.
            }
        }
        // Insert newly created rules.
        for (auto &r : rulesToAdd) {
            rules.push_back(r);
        }
#ifdef DEBUG_LEFT_RECURSION
        std::cout << "Rules after add and delete: " << std::endl;
        printRules();
#endif // DEBUG_LEFT_RECURSION
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
                    lhs); // WARNING: There may be variables whose identifiers
                          // are the same but are inserted more than one time.
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
