#include "lex.hpp"
#include <iostream>
#include <regex>
#include <string>
#include <vector>

// class Lex
std::vector<Terminal *> *Lex::tokenize(std::string str) {
    return tokenize(str.c_str());
}

std::vector<Terminal *> *Lex::tokenize(const char *rawStr) {
    auto tokens = new std::vector<Terminal *>;
    int idx = 0;
    char currCh = rawStr[idx++];
    Terminal whitespace = Terminal(-6, -6, "", std::regex("\\s+"));
    std::vector<Terminal *> candidateMatches = g->terminals;
    candidateMatches.push_back(&whitespace);
    std::string token = "";
    if (verbose) {
        std::cout << "Tokens:" << std::endl;
    }

    // Match longest match
    while (currCh) {
        std::string tmp = token;
        tmp += currCh;
        int cntMatch = 0;

        for (auto &a : candidateMatches) {
            if (a->matcher(tmp)) {
                cntMatch++;
            }
        }

        // If matches, expand token. Else validate token and append it to
        // tokens. If token is invalid, throw exception.
        if (cntMatch) {

            token += currCh;
            currCh = rawStr[idx++];
        } else {

            // Validate token
            Terminal *t = g->matchTerminal(token);
            if (t) {
                if (verbose) {
                    std::cout << token << ": " << t->getIdentifier()
                              << std::endl;
                }
                tokens->push_back(t);
            } else if (!whitespace.matcher(token)) {

                // Skip whitespace in inputStr. If there's any that remains
                // unmatched, it means that token is invalid. throw
                // exception.
                throw std::runtime_error(std::string("ERROR: Invalid token: ") +
                                         token);
            }

            // Initialize token
            token = std::string(1, currCh);
            currCh = rawStr[idx++];
        }

        // Process last token separately
        if (currCh == '\0') {
            Terminal *t = g->matchTerminal(token);
            if (t) {

                if (verbose) {
                    std::cout << token << ": " << t->getIdentifier()
                              << std::endl;
                }
                tokens->push_back(t);
            } else if (!whitespace.matcher(token)) {

                // Skip whitespace in inputStr. If there's any that remains
                // unmatched, it means that token is invalid. throw
                // exception.
                throw std::runtime_error(std::string("ERROR: Invalid token: ") +
                                         token);
            }
        }
    }

    if (verbose) {
        std::cout << std::endl;
    }

    // Append BOTTOM OF STACK symbol to token list.
    tokens->push_back(g->bos);

    return tokens;
}
