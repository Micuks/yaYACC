#include "../src/rule.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

class RuleTest : public ::testing::Test {
  protected:
    void SetUp() {

        Variable *lhs;
        vector<Symbol *> rhs;

        r0_ = new Rule(lhs, rhs);

        lhs = new Variable(-1, -1, "BEGIN");
        r1_ = new Rule(lhs, rhs);

        int cnt = 0;
        Terminal *tmp = new Terminal(cnt, cnt, "t1", std::regex("t1"));
        cnt++;
        rhs.push_back(tmp);
        tmp = new Terminal(cnt, cnt, "t2", std::regex("[0-9]"));
        cnt++;
        rhs.push_back(tmp);
        r2_ = new Rule(lhs, rhs);
    }

    void TearDown() {
        delete r0_;

        for (auto &a : r1_->rhs) {
            if (a->getType() == Symbol::SymbolType::variable) {
                delete (Variable *)a;
            } else if (a->getType() == Symbol::SymbolType::terminal) {
                delete (Terminal *)a;
            }
        }
        delete r1_;

        for (auto &a : r2_->rhs) {
            if (a->getType() == Symbol::SymbolType::variable) {
                delete (Variable *)a;
            } else if (a->getType() == Symbol::SymbolType::terminal) {
                delete (Terminal *)a;
            }
        }
        delete r2_;
    }

    Rule *r0_;
    Rule *r1_;
    Rule *r2_;
};

TEST_F(RuleTest, IsEmptyInitially) {
    EXPECT_EQ(r0_->lhs, nullptr);
    EXPECT_EQ(r0_->rhs.size(), 0);
}

TEST_F(RuleTest, IsLhsCorrect) {
    EXPECT_EQ(r1_->lhs->getIdentifier(), "BEGIN");
    EXPECT_EQ(r1_->rhs.size(), 0);
}

TEST_F(RuleTest, PrintRule) {
    std::cout << "r0_: " << *r0_ << std::endl;
    std::cout << "r1_: " << *r1_ << std::endl;
    std::cout << "r2_: ";
    r2_->printRule();
}
