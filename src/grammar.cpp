#include "grammar.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>

using namespace std;
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

void Grammar::loadGrammar(const char *filename) {
    int cnt = 0;
    Variable *preVar;
    Variable *currVar;
    Rule currRule;

    fstream file;
    file.open(filename, fstream::in);

    if (!file.is_open()) {
        cerr << "Error opening grammar file" << endl;
        exit(EXIT_FAILURE);
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
