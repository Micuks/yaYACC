#include "../googletest/googletest/include/gtest/gtest.h"
#include "../src/symbol.hpp"
#include <gtest/gtest.h>
#include <regex>
#include <string>
#include <vector>

TEST(SymbolTests, VariableCorrectly) {
    std::string identifier("<\(>");
    std::string regex_identifier("\(");
    Variable var(-1, -1, identifier,
                 regex(identifier.substr(1, identifier.length() - 2)));

    ASSERT_EQ(var.getType(), SymbolType(variable));
    ASSERT_EQ(var.getIndex(), -1);
    ASSERT_EQ(var.getIdentifier(), identifier);
    ASSERT_EQ(var.getTag(), -1);
    ASSERT_EQ(var.matcher(regex_identifier), std::regex_match(identifier.substr(1, identifier.length()-2), regex_identifier));
}
