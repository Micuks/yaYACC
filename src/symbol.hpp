#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <regex>
#include <string>
using namespace std;

enum SymbolType { variable, terminal };

class Symbol {
  public:
    Symbol(int tag, int index, string id)
        : tag(tag), index(index), identifier(id) {}
    const string getIdentifier() const { return identifier; }
    const int getTag() const { return tag; }
    const int getIndex() const { return index; }

    bool operator==(const Symbol &rhs) const {
        return (rhs.getIdentifier() == getIdentifier()) &&
               (rhs.getIndex() == getIndex()) && (rhs.getTag() == getTag());
        // TODO: Implement !=
    }

    friend ostream &operator<<(ostream &os, const Symbol &sym);

    virtual SymbolType getType() = 0;
    virtual bool matcher(string token) = 0;
    virtual string toString() const;

  private:
    int tag;
    int index;
    string identifier;
};

class Variable : public Symbol {
  public:
    Variable(int tag, int index, string id) : Symbol(tag, index, id) {}
    SymbolType getType() { return SymbolType(variable); }
    bool matcher(string token) { return -1; } // ?

    friend ostream &operator<<(ostream &os, const Variable &sym);
};

class Terminal : public Symbol {
  public:
    Terminal(int tag, int index, string id, regex pattern)
        : Symbol(tag, index, id), pattern(pattern) {}
    SymbolType getType() { return SymbolType(terminal); }
    bool matcher(string token) { return (regex_match(token, pattern)); }
    friend ostream &operator<<(ostream &os, const Terminal &sym);

    // private:
    regex pattern;
};

#endif // !SYMBOL_HPP
