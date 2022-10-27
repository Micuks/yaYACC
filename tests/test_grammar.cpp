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

TEST_F(GrammarTest, IsGrammarCorrectlyLoad) {
    Variable *v = new Variable(0, 0, "S");
    g1_.printRules();
    EXPECT_EQ(g1_.matchTerminal(std::string("b"))->getIdentifier(), "b");
    EXPECT_EQ(g1_.matchTerminal(std::string("a"))->getIdentifier(), "a");

    EXPECT_EQ(g1_.atLhsRules(v).size(), 1);
    for (auto &a : g1_.atLhsRules(v)) {
        std::cout << a << " ";
    }
    std::cout << std::endl;

    EXPECT_EQ(g1_.variables.size(), 1);
    EXPECT_EQ(g1_.terminals.size(), 2);
    EXPECT_EQ(g1_.rules.size(), 2);

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
}
