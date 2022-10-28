#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP
#include <regex>
#include <vector>

#include "rule.hpp"
#include "symbol.hpp"

using namespace std;

class Grammar {
  public:
    vector<Terminal *> terminals;
    vector<Variable *> variables;
    vector<Rule> rules;

    Variable *startSymbol;
    Terminal *epsilon = new Terminal(-1, -1, "EPSILON", regex(""));
    Terminal *bos = new Terminal(-3, -3, "BOTTOM OF STACK", regex(""));

    vector<Rule> atLhsRules(Variable *v);
    vector<Rule> atRhsRules(Symbol *s);

    void loadGrammar(const char *filename);
    void printRules();
    Symbol *getSymbol(int tag);
    Terminal *matchTerminal(string str);
    bool toEpsilonDirectly(Variable *sym);
};
#endif // !GRAMMAR_HPP
