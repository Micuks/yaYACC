#ifndef RULE_HPP
#define RULE_HPP
#include "main.hpp"
#include "symbol.hpp"
#include <utility>
#include <vector>

class Rule {
  public:
    Rule() : lhs(nullptr) {}
    Rule(Rule &rule) : Rule(rule.lhs, rule.rhs) {}
    Rule(const Rule &rule) : Rule(rule.lhs, rule.rhs) {}
    Rule(Variable *lhs, vector<Symbol *> rhs) : lhs(lhs), rhs(std::move(rhs)) {}

    bool leadToEpsilon(const Symbol *epsilon);

    void printRule();
    std::string toString();

    Variable *lhs;
    std::vector<Symbol *> rhs;
};

ostream &operator<<(ostream &os, const Rule &r);
bool operator==(Rule &ruleA, Rule &ruleB);

class LR1Item : public Rule {
  public:
    LR1Item(Rule &rule, int dotPos, Terminal *lookAhead)
        : Rule(rule), dotPos(dotPos), lookAhead(lookAhead) {}
    LR1Item(LR1Item &old)
        : Rule(old.lhs, old.rhs), dotPos(old.dotPos), lookAhead(old.lookAhead) {
    }
    LR1Item(const LR1Item &old)
        : Rule(old.lhs, old.rhs), dotPos(old.dotPos), lookAhead(old.lookAhead) {
    }

    std::string toString();
    Rule getBase();

    int dotPos;
    Terminal *lookAhead;
};

ostream &operator<<(ostream &os, const LR1Item &item);
bool operator==(LR1Item &itemA, LR1Item &itemB);
bool operator==(const LR1Item &itemA, const LR1Item &itemB);

#endif // !RULE_HPP
