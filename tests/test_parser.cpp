#include "../src/lex.hpp"
#include "../src/main.hpp"
#include "../src/parser.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <unordered_set>
#include <vector>

class ParserTest : public ::testing::Test {
  protected:
    void SetUp() {
        p0_ = new Parser();

        g1_ = new Grammar();
        g1_->loadGrammar("../grammars/g1.txt");
        p1_ = new Parser(g1_, true);
        l1_ = new Lex(g1_, true);

        S = g1_->startSymbol;
        A = (Variable *)g1_->getSymbol(2);
        Ta = g1_->matchTerminal("a");

        g2_ = new Grammar();
        g2_->loadGrammar("../grammars/g2.txt");
        p2_ = new Parser(g2_, true);
        l2_ = new Lex(g2_, true);

        g4_7_ = new Grammar();
        g4_7_->loadGrammar("../grammars/g4.7.txt");
        p4_7_ = new Parser(g4_7_, true);

        g4_4_ = new Grammar();
        g4_4_->loadGrammar("../grammars/g4.4.txt");
        p4_4_ = new Parser(g4_4_, true);
    }

    void TearDown() {
        delete p0_;
        delete g1_;
        delete p1_;
        delete l1_;

        delete g2_;
        delete p2_;
        delete l2_;

        delete g4_7_;
        delete p4_7_;

        delete g4_4_;
        delete p4_4_;
    }

    Parser *p0_;

    Grammar *g1_;
    Parser *p1_;
    Lex *l1_;
    Variable *A;
    Variable *S;
    Terminal *Ta;

    Grammar *g2_;
    Parser *p2_;
    Lex *l2_;

    Grammar *g4_7_;
    Parser *p4_7_;

    Grammar *g4_4_;
    Parser *p4_4_;

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

// TEST_F(ParserTest, isFollowCorrect4_7) {
//     Grammar *g = p4_7_->grammar;
//     g->printRules();
//
//     p4_7_->printFollowTable();
// }

TEST_F(ParserTest, isFirstCorrect4_4) {
    Grammar *g = p4_4_->grammar;
    g->printRules();
    p4_4_->printFirstTable();
}

TEST_F(ParserTest, isFollowCorrect4_4) {
    Grammar *g = p4_4_->grammar;
    g->printRules();

    p4_4_->printFollowTable();
}

TEST_F(ParserTest, isFirstCorrect2) {

    // test for all variables in parser
    Grammar *&g = p2_->grammar;
    g->printRules();

    std::cout << "variables: " << std::endl;
    for (auto &a : g->variables) {
        std::cout << a->toString() << ", ";
    }
    std::cout << std::endl;
    std::cout << "Terminals:" << std::endl;
    for (auto &a : g->terminals) {
        std::cout << a->toString() << ", ";
    }
    std::cout << std::endl << std::endl;

    p2_->printFirstTable();

    std::cout << "\nTarget FIRST(A):" << endl;
    std::cout << "A: +,EPSILON\n"
              << "T: (, [0-9]\n"
              << "B: *,EPSILON\n"
              << "F: (, [0-9]\n"
              << "E: (, [0-9]\n"
              << "N: [0-1]\n"
              << std::endl;

    // Start variable A
    std::unordered_set<Terminal *> ASet = p2_->first(g2_->getSymbol(0));
    std::cout << "FIRST(A): ";
    for (auto &a : ASet) {
        std::cout << (*a).toString() << " ";
    }
    std::cout << "" << std::endl;
    ASSERT_EQ(ASet.size(), 2);
}

TEST_F(ParserTest, isFollowCorrect2) {
    auto &g = p2_->grammar;
    g->printRules();

    p2_->printFollowTable();
}

TEST_F(ParserTest, isMakeTableCorrect1) {
    Parser *p = p1_;
    Grammar *g = p->grammar;
    g->printRules();
    p->printFirstTable();
    p->printFollowTable();

    p->makeTable();
    EXPECT_EQ(p->lenParseTable, 2);
    EXPECT_NE(p->parseTable, nullptr);

    p->printParseTable();
}

TEST_F(ParserTest, isMakeTableCorrect4_4) {
    Parser *p = p4_4_;
    Grammar *g = p->grammar;

    g->printRules();
    std::cout << std::endl;
    p->printFirstTable();
    std::cout << std::endl;
    p->printFollowTable();
    std::cout << std::endl;

    p->makeTable();
    p->printParseTable();
    std::string expectedParseTable(
        "VARIABLE         BOTTOM OF STACK  <\\+>             <\\*>             "
        "<\\(>             <\\)>             <id>             \nE              "
        "  error            error            error            E -> T E'        "
        "error            E -> T E'        \nT                error            "
        "error            error            T -> F T'        error            T "
        "-> F T'        \nE'               E' -> EPSILON    E' -> <\\+> T E'  "
        "error            error            E' -> EPSILON    error            "
        "\nF                error            error            error            "
        "F -> <\\(> E <\\)> error            F -> <id>        \nT'             "
        "  T' -> EPSILON    T' -> EPSILON    T' -> <\\*> F T'  error           "
        " T' -> EPSILON    error            \n");
    // std::cout << expectedParseTable << std::endl;
    EXPECT_EQ(expectedParseTable, p->parseTableToString());
}

// TEST_F(ParserTest, isMakeTableCorrect4_7) {
//     Parser *p = p4_7_;
//     Grammar *g = p->grammar;
//
//     p->makeTable();
//     p->printParseTable();
// }

// TEST_F(ParserTest, isMakeTableCorrect2) {
//     Parser *p = p2_;
//     Grammar *g = p->grammar;
//
//     g->printRules();
//     p->printFirstTable();
//     p->printFollowTable();
//
//     p->makeTable();
//     p->printParseTable();
// }

TEST_F(ParserTest, isParseCorrect1_0) {
    Parser *p = p1_;
    Grammar *g = p->grammar;
    Lex *l = l1_;

    EXPECT_EQ(p->verbose, true);
    ASSERT_NE(g, nullptr);
    ASSERT_NE(g->bos, nullptr);
    ASSERT_EQ(g->startSymbol->getIdentifier(), "S");
    p->makeTable();
    p->printParseTable();

    // Test on str_1_0_;
    std::cout << "Tokenized [\"" << str1_0_ << "\"]:\n";
    std::vector<Terminal *> *tok1_0 = l->tokenize(str1_0_);
    ASSERT_NE(tok1_0, nullptr);
    std::cout << "This input string is expected to be rejected." << std::endl;
    try {
        p->parse(tok1_0);
    } catch (std::exception &e) {
        std::cerr << e.what();
        EXPECT_EQ(
            std::string(e.what()),
            std::string(
                "ERROR: No entry of rule at position 1. REJECT INPUT "
                "STRING.\nCurrent elements in stack (bottom to top):\nBOTTOM "
                "OF STACK, A, \nRemaining unparsed input string:\n<a>, <a>, "
                "<b>, "
                "<b>, <b>, <a>, BOTTOM OF STACK, \n"));
    }
}

TEST_F(ParserTest, isParseCorrect1_1) {
    Parser *p = p1_;
    Grammar *g = p->grammar;
    Lex *l = l1_;

    EXPECT_EQ(p->verbose, true);
    ASSERT_NE(g, nullptr);
    ASSERT_NE(g->bos, nullptr);
    ASSERT_EQ(g->startSymbol->getIdentifier(), "S");
    p->makeTable();
    p->printParseTable();

    // Test on str_1_1_;
    std::cout << "Tokenized[\"" << str1_1_ << "\"]:\n";
    std::vector<Terminal *> *tok1_1 = l->tokenize(str1_1_);
    ASSERT_NE(tok1_1, nullptr);
    try {
        p->parse(tok1_1);
    } catch (std::exception &e) {
        std::cout << e.what();
    }

    // Free memory allocated for tokenized input string.
    delete tok1_1;
}

TEST_F(ParserTest, isParseCorrect2_0) {
    Parser *p = p2_;
    Grammar *g = p->grammar;
    Lex *l = l2_;

    EXPECT_EQ(p->verbose, true);
    ASSERT_NE(g, nullptr);
    ASSERT_NE(g->bos, nullptr);
    ASSERT_EQ(g->startSymbol->getIdentifier(), "E");
    p->makeTable();
    p->printParseTable();

    // Test on str_2_0_;
    std::cout << "Tokenized[\"" << str2_0_ << "\"]:\n";
    std::cout << "This string is expected to be rejected.\n";
    std::vector<Terminal *> *tok2_0 = l->tokenize(str2_0_);
    ASSERT_NE(tok2_0, nullptr);
    try {
        p->parse(tok2_0);
    } catch (std::exception &e) {
        std::cout << e.what();
        ASSERT_EQ(
            std::string(e.what()),
            std::string("ERROR: No entry of rule at position 6. REJECT INPUT "
                        "STRING.\nCurrent elements in stack (bottom to "
                        "top):\nBOTTOM OF STACK, A, T, \nRemaining unparsed "
                        "input string:\nBOTTOM OF STACK, \n"));
    }
    std::cout << std::endl;
}

TEST_F(ParserTest, isParseCorrect2_1) {
    Parser *p = p2_;
    Grammar *g = p->grammar;
    Lex *l = l2_;

    EXPECT_EQ(p->verbose, true);
    ASSERT_NE(g, nullptr);
    ASSERT_NE(g->bos, nullptr);
    ASSERT_EQ(g->startSymbol->getIdentifier(), "E");
    p->makeTable();
    p->printParseTable();

    // Test on str_2_1_;
    std::cout << "Tokenized[\"" << str2_1_ << "\"]:\n";
    std::cout << "This string is expected to be accepted.\n";
    std::vector<Terminal *> *tok2_1 = l->tokenize(str2_1_);
    ASSERT_NE(tok2_1, nullptr);
    try {
        p->parse(tok2_1);
    } catch (std::exception &e) {
        std::cout << e.what();
        ASSERT_EQ(std::string(e.what()), std::string(""));
    }
    std::cout << std::endl;
}

TEST_F(ParserTest, isParseCorrect2_2) {
    Parser *p = p2_;
    Grammar *g = p->grammar;
    Lex *l = l2_;

    EXPECT_EQ(p->verbose, true);
    ASSERT_NE(g, nullptr);
    ASSERT_NE(g->bos, nullptr);
    ASSERT_EQ(g->startSymbol->getIdentifier(), "E");
    p->makeTable();
    p->printParseTable();
    //
    // Test on str_2_2_;
    std::cout << "Tokenized[\"" << str2_2_ << "\"]:\n";
    std::cout << "This string is expected to be rejected for containing "
                 "illegal symbol '/'.\n";
    std::vector<Terminal *> *tok2_2;
    try {
        tok2_2 = l->tokenize(str2_2_);
    } catch (std::exception &e) {
        std::cout << e.what();
        ASSERT_EQ(std::string(e.what()),
                  std::string("ERROR: Invalid token: /"));
    }
    ASSERT_NE(tok2_2, nullptr);
    try {
        p->parse(tok2_2);
    } catch (std::exception &e) {
        std::cout << e.what();
        ASSERT_EQ(std::string(e.what()),
                  "ERROR: No entry of rule at position 0. REJECT INPUT "
                  "STRING.\nCurrent elements in stack (bottom to top):\nBOTTOM "
                  "OF STACK, E, \nRemaining unparsed input string:\nBOTTOM OF "
                  "STACK, <\\+>, <\\*>, <\\(>, <\\)>, <id>, \n");
    }
    std::cout << std::endl;
}

TEST_F(ParserTest, isParseCorrect2_21) {
    Parser *p = p2_;
    Grammar *g = p->grammar;
    Lex *l = l2_;

    EXPECT_EQ(p->verbose, true);
    ASSERT_NE(g, nullptr);
    ASSERT_NE(g->bos, nullptr);
    ASSERT_EQ(g->startSymbol->getIdentifier(), "E");
    p->makeTable();
    p->printParseTable();

    // Test on str_2_21_;
    std::cout << "Tokenized[\"" << str2_21_ << "\"]:\n";
    std::cout << "This string is expected to be accepted.\n";
    std::vector<Terminal *> *tok2_21 = nullptr;
    try {
        tok2_21 = l->tokenize(str2_21_);
    } catch (std::exception &e) {
        std::cout << e.what();
        EXPECT_EQ(std::string(e.what()), std::string(""));
    }
    ASSERT_NE(tok2_21, nullptr);
    try {
        p->parse(tok2_21);
    } catch (std::exception &e) {
        std::cout << e.what();
    }
    std::cout << std::endl;
}

TEST_F(ParserTest, isParseCorrect2_3) {
    Parser *p = p2_;
    Grammar *g = p->grammar;
    Lex *l = l2_;

    EXPECT_EQ(p->verbose, true);
    ASSERT_NE(g, nullptr);
    ASSERT_NE(g->bos, nullptr);
    ASSERT_EQ(g->startSymbol->getIdentifier(), "E");
    p->makeTable();
    p->printParseTable();

    // Test on str_2_3_;
    std::cout << "Tokenized[\"" << str2_3_ << "\"]:\n";
    std::cout
        << "This string is expected to be rejected for unpaired brakets.\n";
    std::vector<Terminal *> *tok2_3 = l->tokenize(str2_3_);
    ASSERT_NE(tok2_3, nullptr);
    try {
        p->parse(tok2_3);
    } catch (std::exception &e) {
        std::cout << e.what();
        EXPECT_EQ(
            std::string(e.what()),
            std::string(
                "ERROR: No entry of rule at position 7. REJECT INPUT "
                "STRING.\nCurrent elements in stack (bottom to top):\nBOTTOM "
                "OF STACK, A, B, \nRemaining unparsed input string:\n<\\(>, "
                "<[0-9]+>, <\\+>, <[0-9]+>, <\\*>, <\\(>, <[0-9]+>, <\\*>, "
                "<[0-9]+>, <\\+>, <[0-9]+>, <\\)>, BOTTOM OF STACK, \n"));
    }
    std::cout << std::endl;
}
