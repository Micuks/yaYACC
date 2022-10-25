#ifndef RULE_HPP
#define RULE_HPP

#include "symbol.hpp"
#include <vector>
using namespace std;

class Rule {
  public:
    Variable *lhs;
    vector<Symbol *> rhs;

    Rule() {}
    Rule(Variable *lhs, vector<Symbol *> rhs) : lhs(lhs), rhs(rhs) {}
};
#endif // !RULE_HPP
