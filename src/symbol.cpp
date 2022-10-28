#include "symbol.hpp"
#include <sstream>

using namespace std;

string Symbol::toString() const {
    stringstream ss;
    ss << "[" << this->getTag() << ", " << this->getIndex() << ", "
       << this->getIdentifier() << "]";
    return ss.str();
}
// Variable
ostream &operator<<(ostream &os, const Variable &sym) {
    os << sym.getIdentifier();
    return os;
}

// Terminal
ostream &operator<<(ostream &os, const Terminal &sym) {
    os << sym.getIdentifier();
    return os;
}
