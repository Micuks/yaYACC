#include "../src/lex.hpp"
#include <algorithm>
#include <exception>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>

class LexTest : public ::testing::Test {
  protected:
    void SetUp() {
        g0_ = new Grammar();
        l0_ = new Lex(g0_, true);

        g1_ = new Grammar();
        g1_->loadGrammar("../grammars/g1.txt");
        l1_ = new Lex(g1_, true);

        g2_ = new Grammar();
        g2_->loadGrammar("../grammars/g2.txt");
        l2_ = new Lex(g2_, true);
    }

    void TearDown() {
        delete g0_;
        delete l0_;

        delete g1_;
        delete l1_;

        delete g2_;
        delete l2_;
    }

    Grammar *g0_;
    Lex *l0_;

    Grammar *g1_;
    Lex *l1_;

    Grammar *g2_;
    Lex *l2_;

    std::string str1_0_ = std::string("aaa bbb a");
    std::string str1_1_ = std::string("aan");

    std::string str2_0_ = std::string("1+2+3+");
    std::string str2_1_ = std::string("423*384*2 3");
    std::string str2_2_ = std::string("(33+34)*45/32+8*(3*1+3)");
    std::string str2_21_ = std::string("(33+34)*45)32+8*(3*1+3)");
    std::string str2_3_ =
        std::string("(33+34)*45(32+8*(3-1+3)"); // Have grammar error that can't
                                                // pass LL(1) parse process.
};

TEST_F(LexTest, isEmptyInitially) { EXPECT_EQ(l0_->g->rules.size(), 0); }

TEST_F(LexTest, tokenize1) {
    std::cout << "Accepted tokens: \n";
    Grammar *&g = l1_->g;

    vector<Terminal *> &ters = g->terminals;
    for (auto &ter : ters) {
        std::cout << ter->toString() << ", ";
    }
    std::cout << std::endl;
    std::cout << "Tokenize [" << str1_0_ << "]:\n";
    std::vector<Terminal *> *tok1_0_ = l1_->tokenize(str1_0_);
    std::cout << "Tokenize [" << str1_1_ << "]:\n";
    std::cout << "This tokenization is expected to throw an exception.\n";
    try {
        std::vector<Terminal *> *tok1_1_ = l1_->tokenize(str1_1_);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        EXPECT_STREQ("ERROR: Invalid token: n", e.what());
    }
}

TEST_F(LexTest, tokenize2) {
    std::cout << "Accepted tokens: \n";
    Grammar *&g = l2_->g;

    vector<Terminal *> &ters = g->terminals;
    for (auto &ter : ters) {
        std::cout << ter->toString() << ", ";
    }
    std::cout << std::endl;

    std::cout << "Tokenize [" << str2_0_ << "]:\n";
    std::vector<Terminal *> *tok2_0_ = l2_->tokenize(str2_0_);

    std::cout << "Tokenize [" << str2_1_ << "]:\n";
    std::vector<Terminal *> *tok2_1_ = l2_->tokenize(str2_1_);

    std::cout << "Tokenize [" << str2_21_ << "]:\n";
    std::vector<Terminal *> *tok2_21_ = l2_->tokenize(str2_21_);

    std::cout << "Tokenize [" << str2_2_ << "]:\n";
    std::cout << "This tokenization is expected to throw an exception.\n";
    try {
        std::vector<Terminal *> *tok2_2_ = l2_->tokenize(str2_2_);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        EXPECT_STREQ(e.what(), "ERROR: Invalid token: /");
    }

    std::cout << "Tokenize [" << str2_3_ << "]:\n";
    std::cout << "This tokenization is expected to throw an exception.\n";
    try {
        std::vector<Terminal *> *tok2_3_ = l2_->tokenize(str2_3_);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        EXPECT_STREQ(e.what(), "ERROR: Invalid token: -");
    }
}
