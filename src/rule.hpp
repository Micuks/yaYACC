#ifndef RULE_HPP
#define RULE_HPP

#include "symbol.hpp"
#include <utility>
#include <vector>
using namespace std;

class Rule {
  public:
    Rule() : lhs(nullptr) {}
    Rule(Variable *lhs, vector<Symbol *> rhs) : lhs(lhs), rhs(std::move(rhs)) {}

    bool leadToEpsilon(const Symbol *epsilon);

    void printRule();
    string toString();

    Variable *lhs;
    vector<Symbol *> rhs;
};

ostream &operator<<(ostream &os, const Rule &r);
#endif // !RULE_HPP
