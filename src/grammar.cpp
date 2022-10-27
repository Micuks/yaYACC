#include "grammar.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>

using namespace std;
vector<Rule> Grammar::atLhsRules(Variable *v) {
    vector<Rule> retVec;
    for (Rule r : rules) {
        if (r.lhs == v) {
            retVec.push_back(r);
        }
    }
    return retVec;
}

vector<Rule> Grammar::atRhsRules(Symbol *s) {
    vector<Rule> retVec;
    for (Rule r : rules) {
        auto a = find(r.rhs.begin(), r.rhs.end(), s);
        if (a != r.rhs.end()) {
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

    string line;
    while (getline(file, line)) {
        if (line.length() == 0) {
            continue;
        }
        // Initialize current rule
        currRule = Rule();

        istringstream ss(line);
        string token;
        string preToken;

        while (ss >> token) {
            if (token == "->") {
                // Previous symbol is lhs of current rule
                bool isExist = false;
                for (auto &a : variables) {
                    if (a->getIdentifier() == preToken) {
                        currRule.lhs = a;
                        isExist = true;
                        break;
                    }
                }
                if (!isExist) {
                    // Lhs symbol not added to variables vector yet.
                    //     Add it.
                    auto sym = new Variable(cnt++, variables.size(), token);
                    currRule.lhs = sym;
                }
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
            preToken = token;
        }
        // End of rule, add it to ruleset
        rules.push_back(currRule);
    }
    // Set start symbol
    startSymbol = variables[0];
    // Add bottom of stack symbol to terminals
    terminals.push_back(bos);
    file.close();
}

void Grammar::printRules() {
    int cnt = 0;
    cout << "Rules:" << endl;
    for (auto &a : rules) {
        cout << cnt++ << ": ";
        a.printRule();
    }
    cout << endl;
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
    cout << terminals.size();
    // for (auto &a : terminals) {
    //     cout << a->getIdentifier() << " ";
    //     if (a->matcher(str)) {
    //         return a;
    //     }
    // }
    return nullptr;
}
