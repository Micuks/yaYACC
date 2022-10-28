#include "../src/grammar.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <vector>

class GrammarTest : public ::testing::Test {
  protected:
    void SetUp() { g1_.loadGrammar("../grammars/g1.txt"); }

    Grammar g0_;
    Grammar g1_;
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
    auto atRhsRules = g1_.atRhsRules(A);
    EXPECT_EQ(atRhsRules.size(), 1);

    for (auto &a : g1_.atRhsRules(A)) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    Terminal *a = new Terminal(1, 0, "<a>", std::regex("a"));
    atRhsRules = g1_.atRhsRules(a);
    EXPECT_EQ(atRhsRules.size(), 1);

    for (auto &a : g1_.atRhsRules(A)) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    delete A;
    delete a;
}

TEST_F(GrammarTest, checkGetSymbol) {
    EXPECT_EQ(g1_.getSymbol(-1)->getIdentifier(), "EPSILON");
    EXPECT_EQ(g1_.getSymbol(0)->getIdentifier(), "S");
    EXPECT_NE(g1_.getSymbol(10)->getIdentifier(), "A");
}

TEST_F(GrammarTest, checkMatchTerminal0) {
    auto bos = g1_.matchTerminal(std::string("BOTTOM OF STACK"));
    ASSERT_NE(bos, nullptr);
    EXPECT_EQ(bos->getIdentifier(), "BOTTOM OF STACK");

    auto b = g1_.matchTerminal(std::string("b"));
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->getIdentifier(), "b");

    auto a = g1_.matchTerminal(std::string("a"));
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(g1_.matchTerminal(std::string("a"))->getIdentifier(), "a");
}

TEST_F(GrammarTest, checkMatchTerminal1) {
    // TODO: Implement it
    auto t0 = g1_.getSymbol(2);
    auto S = g1_.getSymbol(0);
    EXPECT_EQ(g1_.matchTerminal("a"), t0);
    EXPECT_EQ(g1_.matchTerminal("S"), nullptr);
    // EXPECT_NE(g1_.matchTerminal("S"), S);
}

TEST_F(GrammarTest, checkToEpsilonDIrectly) {
    // TODO: Implement i
    auto A = g1_.getSymbol(1);
    auto S = g1_.getSymbol(0);
    std::cout << "A: " << A << " S: " << S << std::endl;
    EXPECT_TRUE(g1_.toEpsilonDirectly((Variable *)A));
    EXPECT_FALSE(g1_.toEpsilonDirectly((Variable *)S));
}
