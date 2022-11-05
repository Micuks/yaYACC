#include "rule.hpp"
#include <iostream>
#include <sstream>
using namespace std;

// Rule
void Rule::printRule() { cout << *this; }

bool Rule::leadToEpsilon(const Symbol *epsilon) {
    return (rhs.size() == 1) && (rhs.front() == epsilon);
}

string Rule::toString() {
    stringstream ss;
    ss << *this;
    return ss.str();
}

ostream &operator<<(ostream &os, const Rule &r) {
    stringstream ss;
    ss << r.lhs->getIdentifier() << " -> ";
    int cnt2 = 0;

    for (const auto &b : r.rhs) {
        ss << b->getIdentifier();
        if (++cnt2 < r.rhs.size()) {
            ss << " ";
        }
    }

    os << ss.str();
    return os;
}

bool operator==(Rule &ruleA, Rule &ruleB) {
    if (*(ruleA.lhs) == *(ruleB.lhs)) {
        if (ruleA.rhs.size() == ruleB.rhs.size()) {
            for (int i = 0; i < ruleA.rhs.size(); i++) {
                if (ruleA.rhs[i] != ruleB.rhs[i]) {
                    return false;
                }
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;
}

// LR1Item

/**
 * Stream output LR1Item.
 */
ostream &operator<<(ostream &os, const LR1Item &item) {
    stringstream ss;
    ss << item.lhs->getIdentifier() << " -> ";
    for (int i = 0; i < item.rhs.size(); i++) {
        if (i == item.dotPos) {
            ss << "~ ";
        }
        ss << item.rhs[i] << " ";
    }
    if (item.dotPos == item.rhs.size()) {
        ss << "~ ";
    }
    ss << ", " << item.lookAhead;
    os << ss.str();
    return os;
}

bool operator==(LR1Item &itemA, LR1Item &itemB) {
    auto ruleA = itemA.getBase();
    auto ruleB = itemB.getBase();
    if (ruleA == ruleB) {
        return (itemA.dotPos == itemB.dotPos) &&
               (*(itemA.lookAhead) == *(itemB.lookAhead));
    }
    return false;
}

bool operator==(const LR1Item &itemA, const LR1Item &itemB) {
    return itemA == itemB;
}

Rule LR1Item::getBase() { return Rule(lhs, rhs); }
