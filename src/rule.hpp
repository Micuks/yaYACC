#ifndef RULE_HPP
#define RULE_HPP

#include "symbol.hpp"
#include <utility>
#include <vector>
using namespace std;

class Rule {
  public:
    Variable *lhs;
    vector<Symbol *> rhs;

    Rule() : lhs(nullptr) {}
    Rule(Variable *lhs, vector<Symbol *> rhs) : lhs(lhs), rhs(std::move(rhs)) {}
    void printRule();
    // string toString();
};

ostream &operator<<(ostream &os, const Rule &r);
#endif // !RULE_HPP
