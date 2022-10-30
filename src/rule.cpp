#include "rule.hpp"
#include <iostream>
#include <sstream>
using namespace std;

void Rule::printRule() { cout << *this; }

bool Rule::leadToEpsilon(const Symbol *epsilon) {
    return (rhs.size() == 1) && (rhs.front() == epsilon);
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

string Rule::toString() {
    stringstream ss;
    ss << *this;
    return ss.str();
}
