#ifndef LEX_HPP
#define LEX_HPP

#include "grammar.hpp"
#include <string>
#include <vector>

class Lex {
  public:
    Lex(Grammar *g, bool v) : g(g), verbose(v) {}

    std::vector<Terminal *> *tokenize(const char *str);
    std::vector<Terminal *> *tokenize(std::string str);

    // private:
    Grammar *g;
    bool verbose;
};
#endif // !LEX_HPP
