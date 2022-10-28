#include "rule.hpp"
#include <iostream>
using namespace std;

void Rule::printRule() { cout << *this; }

// string Rule::toString() {
//
// }

ostream &operator<<(ostream &os, const Rule &r) {
    os << r.lhs->getIdentifier() << " -> ";
    int cnt2 = 0;
    for (const auto &b : r.rhs) {
        cout << b->getIdentifier();
        if (++cnt2 < r.rhs.size()) {
            cout << " ";
        }
    }
    os << endl;
    return os;
}
