#include "../src/grammar.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <vector>

//
// Created by 吴清柳 on 10/27/22.
//
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
        std::cout << a->getIdentifier() << " ";
    }
    std::cout << std::endl;

    std::cout << "g1_.terminals: \n";
    for (auto &a : g1_.terminals) {
        std::cout << a->getIdentifier() << " ";
    }
    std::cout << std::endl;

    g1_.printRules();

    EXPECT_EQ(g1_.variables.size(), 1);
    EXPECT_EQ(g1_.terminals.size(), 2);
    EXPECT_EQ(g1_.rules.size(), 2);
}

TEST_F(GrammarTest, checkMatchTerminal) {
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

TEST_F(GrammarTest, checkAtLhsRules) {
    Variable *v = new Variable(0, 0, "S");
    EXPECT_EQ(g1_.atLhsRules(v).size(), 1);
    for (auto &a : g1_.atLhsRules(v)) {
        std::cout << a << " ";
    }
    std::cout << std::endl;
}

TEST_F(GrammarTest, checkAtRhsRules) { EXPECT_EQ("TODO", "Check atRhsRules"); }

TEST_F(GrammarTest, checkGetSymbol) {
    EXPECT_EQ(g1_.getSymbol(-1)->getIdentifier(), "EPSILON");
    EXPECT_EQ(g1_.getSymbol(0)->getIdentifier(), "S");
    EXPECT_NE(g1_.getSymbol(10)->getIdentifier(), "A");
}
