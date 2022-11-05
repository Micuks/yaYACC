#include "../src/grammar.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <vector>

class GrammarTest : public ::testing::Test {
  protected:
    void SetUp() {
        g1_.loadGrammar("../grammars/g1.txt");
        // g1_.printRules();

        g2_.loadGrammar("../grammars/g2.txt");
        // g2_.printRules();

        // Test convert cfg to ll(1) acceptable grammar.
        g3_.loadGrammar("../grammars/g3.txt");
        // g3_.printRules();
    }

    Grammar g0_;
    Grammar g1_;
    Grammar g2_;
    Grammar g3_;
};

TEST_F(GrammarTest, IsEmptyInitially) {
    EXPECT_EQ(g0_.startSymbol, nullptr);
    EXPECT_EQ(g0_.epsilon->getIdentifier(), "EPSILON");
}

TEST_F(GrammarTest, checkTerminalsAndVariables) {
    std::cout << "g1_.variables: \n";
    for (auto &a : g1_.variables) {
        std::cout << "[" << a->getTag() << ", " << a->getIndex() << ", "
                  << a->getIdentifier() << "], ";
    }
    std::cout << std::endl;

    std::cout << "g1_.terminals: \n";
    for (auto &a : g1_.terminals) {
        std::cout << "[" << a->getTag() << ", " << a->getIndex() << ", "
                  << a->getIdentifier() << "], ";
    }
    std::cout << std::endl;

    g1_.printRules();

    EXPECT_EQ(g1_.startSymbol->getIdentifier(), "S");
    EXPECT_EQ(g1_.variables.size(), 2);
    EXPECT_EQ(g1_.terminals.size(), 3);
    EXPECT_EQ(g1_.rules.size(), 3);
}

TEST_F(GrammarTest, checkAtLhsRules) {
    Variable *v = new Variable(0, 0, "S");
    auto atLhsRules = g1_.atLhsRules(v);
    EXPECT_EQ(atLhsRules.size(), 1);
    for (auto &a : g1_.atLhsRules(v)) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    delete v;
}

TEST_F(GrammarTest, checkAtRhsRules) {
    Variable *A = new Variable(2, 1, "A");

    std::cout << "------" << std::endl;
    for (auto &a : g1_.atRhsRules(A)) {
        std::cout << a << std::endl;
    }
    std::cout << "------" << std::endl;

    auto atRhsRules = g1_.atRhsRules(A);
    EXPECT_EQ(atRhsRules.size(), 1);

    Terminal *a = new Terminal(1, 1, "<a>", std::regex("a"));

    std::cout << "------" << std::endl;
    for (auto &a : g1_.atRhsRules(a)) {
        std::cout << a << std::endl;
    }
    std::cout << "------" << std::endl;

    atRhsRules = g1_.atRhsRules(a);
    EXPECT_EQ(atRhsRules.size(), 1);

    delete A;
    delete a;
}

TEST_F(GrammarTest, checkGetSymbol) {
    auto sm1 = g1_.getSymbol(-1);
    ASSERT_NE(sm1, nullptr);
    EXPECT_EQ(g1_.getSymbol(-1)->getIdentifier(), "EPSILON");

    auto s0 = g1_.getSymbol(0);
    ASSERT_NE(s0, nullptr);
    EXPECT_EQ(s0->getIdentifier(), "S");

    auto s10 = g1_.getSymbol(10);
    ASSERT_EQ(s10, nullptr);

    auto s2 = g1_.getSymbol(2);
    EXPECT_EQ(s2->getIdentifier(), "A");

    auto s1 = g1_.getSymbol(1);
    EXPECT_EQ(s1->getIdentifier(), "<a>");
}

TEST_F(GrammarTest, checkMatchTerminal0) {
    auto bos = g1_.matchTerminal(std::string("BOTTOM OF STACK"));
    ASSERT_EQ(bos, nullptr);

    auto b = g1_.matchTerminal(std::string("b"));
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->getIdentifier(), "<b>");

    auto a = g1_.matchTerminal(std::string("a"));
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(g1_.matchTerminal(std::string("a"))->getIdentifier(), "<a>");
}

TEST_F(GrammarTest, checkMatchTerminal1) {
    auto t0 = g1_.getSymbol(1);
    auto S = g1_.getSymbol(0);
    EXPECT_EQ(*(g1_.matchTerminal("a")), *t0);
    EXPECT_EQ(g1_.matchTerminal("S"), nullptr);
    // EXPECT_NE(g1_.matchTerminal("S"), S);
}

TEST_F(GrammarTest, checkToEpsilonDIrectly) {
    auto A = g1_.getSymbol(2);
    auto S = g1_.getSymbol(0);
    std::cout << "A: " << *A << " S: " << *S << std::endl;
    EXPECT_TRUE(g1_.toEpsilonDirectly((Variable *)A));
    EXPECT_FALSE(g1_.toEpsilonDirectly((Variable *)S));
}

TEST_F(GrammarTest, checkEliminataLeftRecursion) {
    Grammar g = g3_;
    g.printRules();
    g.cfg2LL1();
    //
    // std::cout << "g3_.variables: \n";
    // for (auto &a : g3_.variables) {
    //     std::cout << "[" << a->getTag() << ", " << a->getIndex() << ", "
    //               << a->getIdentifier() << "], ";
    // }
    // std::cout << std::endl;
    //
    // std::cout << "g3_.terminals: \n";
    // for (auto &a : g3_.terminals) {
    //     std::cout << "[" << a->getTag() << ", " << a->getIndex() << ", "
    //               << a->getIdentifier() << "], ";
    // }
    // std::cout << std::endl;
    //
    g.printRules();
}
