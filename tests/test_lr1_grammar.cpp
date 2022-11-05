#include "../src/grammar.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <vector>

class LR1GrammarTest : public ::testing::Test {
  protected:
    void SetUp() {
        g1_.loadGrammar("../grammars/g1.txt");
        // g1_.printRules();
        lrg1 = LR1Grammar(g1_);

        g2_.loadGrammar("../grammars/g2.txt");
        // g2_.printRules();
        lrg2 = LR1Grammar(g2_);

        // Test convert cfg to ll(1) acceptable grammar.
        g3_.loadGrammar("../grammars/g3.txt");
        // g3_.printRules();
        lrg3 = LR1Grammar(g3_);
    }

    Grammar g1_;
    Grammar g2_;
    Grammar g3_;

    LR1Grammar lrg1;
    LR1Grammar lrg2;
    LR1Grammar lrg3;
};

TEST_F(LR1GrammarTest, testConstructor) {
    Grammar &g = g1_;
    LR1Grammar &lrg = lrg1;

    std::cout << "\ng.variables:\n";
    for (auto &a : g.variables) {
        std::cout << a->toString() << ", ";
    }

    std::cout << std::endl;

    std::cout << "\nLRg.variables:\n";
    for (auto &a : lrg.variables) {
        std::cout << a->toString() << ", ";
    }
    std::cout << std::endl;

    std::cout << "\ng.terminals:\n";
    for (auto &a : g.terminals) {
        std::cout << a->toString() << ", ";
    }

    std::cout << std::endl;

    std::cout << "\nLRg.terminals:\n";
    for (auto &a : lrg.terminals) {
        std::cout << a->toString() << ", ";
    }
    std::cout << std::endl;

    EXPECT_EQ(lrg.variables.size(), g.variables.size() + 1);
    EXPECT_EQ(lrg.terminals.size(), g.terminals.size());

    ASSERT_NE(lrg.startSymbol, nullptr);
    ASSERT_NE(lrg.epsilon, nullptr);
    ASSERT_NE(lrg.bos, nullptr);

    EXPECT_EQ((lrg.startSymbol)->getIdentifier(),
              (g.startSymbol)->getIdentifier() + "'");

    EXPECT_EQ(*(lrg.epsilon), *(g.epsilon));

    EXPECT_EQ(*(lrg.bos), *(g.bos));
}

TEST_F(LR1GrammarTest, augmenting) {
    Grammar &g = g2_;
    LR1Grammar &lrg = lrg2;
    g.printRules();

    bool isNewStartSymbolFound = false;
    EXPECT_EQ(lrg.variables.size(), g.variables.size() + 1);

    for (const auto &a : lrg.variables) {
        if (a->getIdentifier() == "E'")
            isNewStartSymbolFound = true;
    }

    EXPECT_TRUE(isNewStartSymbolFound);
    lrg.printRules();
}
