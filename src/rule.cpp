#include "rule.hpp"
#include <iostream>
#include <sstream>
using namespace std;

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
    // os << endl;
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
