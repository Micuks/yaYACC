#include "../src/symbol.hpp"
#include <gtest/gtest.h>
#include <regex>
#include <string>
#include <vector>
#include <iostream>

TEST(SymbolTests, VariableCorrectly) {
    std::string identifier("<\(>");
    std::string regex_identifier("<[0-9]>");

    Variable var(-1, -1, identifier);
    Terminal ter(-3, -3, "BOTTOM OF STACK", std::regex(regex_identifier));

    EXPECT_EQ(var.getType(), SymbolType(variable));
    EXPECT_EQ(var.getIndex(), -1);
    EXPECT_EQ(var.getIdentifier(), identifier);
    EXPECT_EQ(var.getTag(), -1);
}

TEST(SymbolTests, TerminalCorrectly) {
    std::string identifier("<\(>");
    std::string regex_identifier("<[0-9]>");

    Variable var(-1, -1, identifier);
    Terminal ter(-3, -3, "BOTTOM OF STACK", std::regex(regex_identifier));

    EXPECT_EQ(ter.getType(), SymbolType(terminal));
    EXPECT_EQ(ter.getIndex(), -3);
    EXPECT_EQ(ter.getIdentifier(), "BOTTOM OF STACK");
    EXPECT_EQ(ter.getTag(), -3);
    EXPECT_EQ(ter.matcher("34"), std::regex_match("34", std::regex(regex_identifier)));
}
