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

        g2_ = new Grammar();
        g2_->loadGrammar("../grammars/g2.txt");
        p2_ = new Parser(g2_, true);

        g4_7_ = new Grammar();
        g4_7_->loadGrammar("../grammars/g4.7.txt");
        p4_7_ = new Parser(g4_7_, true);
    }

    void TearDown() {
        delete p0_;
        delete g1_;
        delete p1_;

        delete g2_;
        delete p2_;

        delete g4_7_;
        delete p4_7_;
    }

    Parser *p0_;

    Grammar *g1_;
    Parser *p1_;
    Variable *A;
    Variable *S;
    Terminal *Ta;

    Grammar *g2_;
    Parser *p2_;

    Grammar *g4_7_;
    Parser *p4_7_;
};

TEST_F(ParserTest, isEmptyInitially) {
    EXPECT_EQ(p0_->parseTable, nullptr);
    EXPECT_EQ(p0_->grammar, nullptr);
}

TEST_F(ParserTest, isGrammarLoadedCorrectly) {
    Grammar *&g = p1_->grammar;
    g->printRules();

    EXPECT_EQ(g->startSymbol->getIdentifier(), "S");

    p2_->grammar->printRules();
}

TEST_F(ParserTest, isFirstCorrect0) {
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
    std::unordered_set<Terminal *> aSet = p1_->follow(Ta);

    // Terminal EPSILON
    std::cout << "FOLLOW(epsilon): ";
    for (auto &a : epsSet) {
        std::cout << *a << " ";
    }
    std::cout << std::endl;

    EXPECT_EQ(epsSet.size(), 1);
    auto bosInSSet = std::find(epsSet.begin(), epsSet.end(), g->bos);
    ASSERT_NE(bosInSSet, epsSet.end());
    EXPECT_EQ((*bosInSSet)->getIdentifier(), "BOTTOM OF STACK");

    // Terminal a
    std::cout << "FOLLOW(a): ";
    for (auto &a : aSet) {
        std::cout << *a << " ";
    }
    std::cout << std::endl;

    EXPECT_EQ(aSet.size(), 1);
    auto bosInASet = std::find(aSet.begin(), aSet.end(), g->bos);
    ASSERT_NE(bosInASet, aSet.end());
    EXPECT_EQ((*bosInASet)->getIdentifier(), "BOTTOM OF STACK");
}

TEST_F(ParserTest, isFollowCorrect0) {
    std::unordered_set<Terminal *> sSet = p1_->follow(S);
    std::unordered_set<Terminal *> ASet = p1_->follow(A);
    auto &g = p1_->grammar;

    p1_->printFollowTable();

    // Start Symbol S
    std::cout << "FOLLOW(S): ";
    for (auto &a : sSet) {
        std::cout << *a << " ";
    }
    std::cout << std::endl;
    EXPECT_EQ(sSet.size(), 1);
    auto bosInSSet = std::find(sSet.begin(), sSet.end(), g->bos);
    ASSERT_NE(bosInSSet, sSet.end());
    EXPECT_EQ((*bosInSSet)->getIdentifier(), "BOTTOM OF STACK");

    // Non-Terminal variable A
    std::cout << "FOLLOW(A): ";
    for (auto &a : ASet) {
        std::cout << *a << " ";
    }
    std::cout << std::endl;
    EXPECT_EQ(ASet.size(), 1);
    auto bosInASet = std::find(ASet.begin(), ASet.end(), g->bos);
    ASSERT_NE(bosInASet, ASet.end());
    EXPECT_EQ((*bosInASet)->getIdentifier(), "BOTTOM OF STACK");
}

TEST_F(ParserTest, isFirstCorrect4_7) {
    Grammar *g = p4_7_->grammar;
    p4_7_->printFirstTable();
}

TEST_F(ParserTest, isFollowCorrect4_7) {
    Grammar *g = p4_7_->grammar;
    g->printRules();

    p4_7_->printFollowTable();
}

// TEST_F(ParserTest, isFirstCorrect2) {
//
//     // test for all variables in parser
//     Grammar *&g = p2_->grammar;
//     g->printRules();
//
//     std::cout << "variables: " << std::endl;
//     for (auto &a : g->variables) {
//         std::cout << a->toString() << ", ";
//     }
//     std::cout << std::endl;
//     std::cout << "Terminals:" << std::endl;
//     for (auto &a : g->terminals) {
//         std::cout << a->toString() << ", ";
//     }
//     std::cout << std::endl << std::endl;
//
//     p2_->printFirstTable();
//
//     std::cout << "\nTarget FIRST(A):" << endl;
//     std::cout << "A: +,EPSILON\n"
//               << "T: (, [0-9]\n"
//               << "B: *,EPSILON\n"
//               << "F: (, [0-9]\n"
//               << "E: (, [0-9]\n"
//               << "N: [0-1]\n"
//               << std::endl;
//
//     // Start variable A
//     std::unordered_set<Terminal *> ASet = p2_->first(g2_->getSymbol(0));
//     std::cout << "FIRST(A): ";
//     for (auto &a : ASet) {
//         std::cout << (*a).toString() << " ";
//     }
//     std::cout << "" << std::endl;
// }

// TEST_F(ParserTest, isFollowCorrect2) {
//     std::unordered_set<Terminal *> sSet = p2_->follow(S);
//     std::unordered_set<Terminal *> ASet = p2_->follow(A);
//     auto &g = p2_->grammar;
//
//     g->printRules();
//
//     p2_->printFollowTable();
//
//     // Start Symbol S
//     std::cout << "FOLLOW(S): ";
//     for (auto &a : sSet) {
//         std::cout << *a << " ";
//     }
//     std::cout << std::endl;
//     EXPECT_EQ(sSet.size(), 1);
//     auto bosInSSet = std::find(sSet.begin(), sSet.end(), g->bos);
//     ASSERT_NE(bosInSSet, sSet.end());
//     EXPECT_EQ((*bosInSSet)->getIdentifier(), "BOTTOM OF STACK");
//
//     // Non-Terminal variable A
//     std::cout << "FOLLOW(A): ";
//     for (auto &a : ASet) {
//         std::cout << *a << " ";
//     }
//     std::cout << std::endl;
//     EXPECT_EQ(ASet.size(), 1);
//     auto bosInASet = std::find(ASet.begin(), ASet.end(), g->bos);
//     ASSERT_NE(bosInASet, ASet.end());
//     EXPECT_EQ((*bosInASet)->getIdentifier(), "BOTTOM OF STACK");
//
//     // TODO: Write another test for a more complex grammar
// }
