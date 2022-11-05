#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP
#include "main.hpp"
#include <iostream>
#include <regex>
#include <unordered_set>
#include <vector>

#include "rule.hpp"
#include "symbol.hpp"

class Grammar {
  public:
    Grammar() : startSymbol(nullptr) {}
    Grammar(Grammar &g)
        : terminals((g.terminals)), variables((g.variables)), rules((g.rules)),
          startSymbol(g.startSymbol), epsilon(g.epsilon), bos(g.bos) {}
    vector<Terminal *> terminals;
    vector<Variable *> variables;
    vector<Rule> rules;

    Variable *startSymbol;
    Terminal *epsilon = new Terminal(-1, -1, "EPSILON", regex(""));
    Terminal *bos = new Terminal(-2, 0, "BOTTOM OF STACK", regex(""));

    vector<Rule> atLhsRules(Variable *v);
    vector<Rule> atRhsRules(Symbol *s);

    void loadGrammar(const char *filename);
    void loadGrammar(const std::string &filename);
    void printRules();
    Symbol *getSymbol(int tag);
    Terminal *matchTerminal(string str);
    bool toEpsilonDirectly(Variable *sym);

    void cfg2LL1();

  private:
    bool collectVariablesHaveLeftRecursion(
        std::unordered_set<Variable *> &variablesWhoseRulesHaveLeftRecursion);

    Variable *
    getNewVariable(std::unordered_set<Variable *> &newlyAddedVariables,
                   Variable *lhs, int &tagCnt, int &variablesIndexCnt);
};

class LL1Grammar : public Grammar {
  public:
    LL1Grammar() : Grammar() {}
    // LL1 grammer need to eliminate left recursion.
    LL1Grammar(Grammar &g) : Grammar(g) { cfg2LL1(); }
};

class LR1Grammar : public Grammar {
  public:
    LR1Grammar() : Grammar() {}
    LR1Grammar(Grammar &g) : Grammar(g) { augmenting(); }

  private:
    void augmenting();
};
#endif // !GRAMMAR_HPP
