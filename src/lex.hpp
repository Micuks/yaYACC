#ifndef LEX_HPP
#define LEX_HPP

#include "grammar.hpp"
#include <string>
#include <vector>

class Lex {
  public:
    Lex(Grammar *g = nullptr, bool v = false) : g(g), verbose(v) {}

    std::vector<Terminal *> *tokenize(const char *str);
    std::vector<Terminal *> *tokenize(std::string str);

    Grammar *g;

  private:
    bool verbose;
};
#endif // !LEX_HPP
