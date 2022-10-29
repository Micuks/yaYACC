#include "../src/parser.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <unordered_set>

class ParserTest : public ::testing::Test {
  protected:
    void SetUp() {
        p0_ = new Parser();

        g1_ = new Grammar();
        g1_->loadGrammar("../grammars/g1.txt");
        p1_ = new Parser(g1_, true);

        S = g1_->startSymbol;
        A = (Variable *)g1_->getSymbol(2);
        Ta = g1_->matchTerminal("a");
    }

    void TearDown() {
        delete p0_;
        delete g1_;
        delete p1_;
    }

    Parser *p0_;

    Grammar *g1_;
    Parser *p1_;

    Variable *A;
    Variable *S;
    Terminal *Ta;
};

TEST_F(ParserTest, isEmptyInitially) {
    EXPECT_EQ(p0_->parseTable, nullptr);
    EXPECT_EQ(p0_->grammar, nullptr);
}

TEST_F(ParserTest, isGrammarLoadedCorrectly) {
    Grammar *&g = p1_->grammar;
    g->printRules();

    EXPECT_EQ(g->startSymbol->getIdentifier(), "S");
}

TEST_F(ParserTest, isFirstCorrect) {
    std::unordered_set<Terminal *> sSet = p1_->first(S);
    std::unordered_set<Terminal *> ASet = p1_->first(A);
    Grammar *&g = p1_->grammar;
    std::unordered_set<Terminal *> epsSet = p1_->first(g->epsilon);
    std::unordered_set<Terminal *> aSet = p1_->first(Ta);

    p1_->printFirstTable();

    std::unordered_set<Terminal *> target_ASet;
    target_ASet.insert(g->epsilon);
    target_ASet.insert(Ta);

    // Start Symbol S
    std::cout << "FIRST(S): ";
    for (auto &a : sSet) {
        std::cout << (*a).toString() << " ";
    }
    std::cout << "" << std::endl;
    EXPECT_EQ(sSet.size(), 1);
    EXPECT_EQ(**(sSet.begin()), *Ta);

    // Non-Terminal variable A
    std::cout << "FIRST(A): ";
    for (auto &a : ASet) {
        std::cout << (*a).toString() << " ";
    }
    std::cout << "" << std::endl;
    EXPECT_EQ(ASet.size(), 2);

    // Terminal a
    std::cout << "FIRST(a): ";
    for (auto &a : aSet) {
        std::cout << *a << std::endl;
    }
    std::cout << std::endl;
    EXPECT_EQ(aSet.size(), 1);
}

TEST_F(ParserTest, isFollowEmptyCorrect) {
    auto &g = p1_->grammar;
    std::unordered_set<Terminal *> epsSet = p1_->follow(g->epsilon);
    std::unordered_set<Terminal *> aSet = p1_->first(Ta);

    // Terminal EPSILON
    for (auto &a : epsSet) {
        std::cout << a << " ";
    }
    std::cout << std::endl;
    // TODO: Write test for epsilon in ParserTest follow
    // EXPECT_EQ(sSet.size(), 1);
    // auto bosInSSet = std::find(sSet.begin(), sSet.end(), g.bos);
    // ASSERT_NE(bosInSSet, nullptr);
    // EXPECT_EQ(*bosInSSet.getIdentifier(), "BOTTOM OF STACK");

    // Terminal a
    for (auto &a : aSet) {
        std::cout << a << " ";
    }
    std::cout << std::endl;
    // TODO: Write test for terminal a in ParserTest follow
    // EXPECT_EQ(aSet.size(), 1);
    // auto bosInASet = std::find(aSet.begin(), aSet.end(), g.bos);
    // ASSERT_NE(bosInASet, nullptr);
    // EXPECT_EQ(*bosInASet.getIdentifier(), "BOTTOM OF STACK");
}

TEST_F(ParserTest, isFollowCorrect) {
    std::unordered_set<Terminal *> sSet = p1_->follow(S);
    std::unordered_set<Terminal *> ASet = p1_->follow(A);
    auto &g = p1_->grammar;

    p1_->printFollowTable();

    // Start Symbol S
    for (auto &a : sSet) {
        std::cout << a << " ";
    }
    std::cout << std::endl;
    EXPECT_EQ(sSet.size(), 1);
    auto bosInSSet = std::find(sSet.begin(), sSet.end(), g->bos);
    ASSERT_NE(bosInSSet, sSet.end());
    EXPECT_EQ((*bosInSSet)->getIdentifier(), "BOTTOM OF STACK");

    // Non-Terminal variable A
    for (auto &a : ASet) {
        std::cout << a << " ";
    }
    std::cout << std::endl;
    EXPECT_EQ(ASet.size(), 1);
    auto bosInASet = std::find(ASet.begin(), ASet.end(), g->bos);
    ASSERT_NE(bosInASet, ASet.end());
    EXPECT_EQ((*bosInASet)->getIdentifier(), "BOTTOM OF STACK");

    // TODO: Write another test for a more complex grammar
}
