#include "../src/lex.hpp"
#include "../src/main.hpp"
#include "../src/parser.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <unordered_set>
#include <vector>

class LR1ParserTest : public ::testing::Test {
  protected:
    void SetUp() {
        g1_ = new Grammar();
        g1_->loadGrammar("../grammars/g1.txt");
        p1_ = new Parser(g1_, true);
        lp1 = LR1Parser(g1_, true);
        l1_ = new Lex(g1_, true);

        g2_ = new Grammar();
        g2_->loadGrammar("../grammars/g2.txt");
        p2_ = new Parser(g2_, true);
        lp2 = LR1Parser(g2_, true);
        l2_ = new Lex(g2_, true);
    }

    // There is memory leak for i didn't free allocated memory for tight time.

    Grammar *g1_;
    Parser *p1_;
    LR1Parser lp1;
    Lex *l1_;

    Variable *A;
    Variable *S;
    Terminal *Ta;

    Grammar *g2_;
    Parser *p2_;
    LR1Parser lp2;
    Lex *l2_;

    std::string str1_0_ = std::string("aaa bbb a");
    std::string str1_1_ = std::string("  a b   ");

    std::string str2_0_ = std::string("1+2+3+");
    std::string str2_1_ = std::string("423*384*23");
    std::string str2_2_ = std::string("(33+34)*45/32+8*(3*1+3)");
    std::string str2_21_ = std::string("(33+34)*45*32+8*(3*1+3)");
    std::string str2_3_ =
        std::string("(33+34)*45(32+8*(3*1+3)"); // Have grammar error that can't
                                                // pass LL(1) parse process.
};

TEST_F(LR1ParserTest, isGrammarLoadedCorrectly) {
    Grammar *g = lp1.grammar;
    g->printRules();

    EXPECT_EQ(g->startSymbol->getIdentifier(), "S'");

    g = lp2.grammar;
    g->printRules();
}

TEST_F(LR1ParserTest, generateLR1DFAAndParseTable) {
    LR1Parser lp = lp2;
    lp.parse(nullptr);
    lp.printLR1ItemSets(std::cout);
    lp.printLR1ParseTable(std::cout);
}
