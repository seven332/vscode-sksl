#include "golden_test.h"

#include <gtest/gtest.h>
#include <src/sksl/SkSLOperator.h>

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include "overloaded.h"
#include "token.h"

std::string ReadSkSL(const char* source_path) {
    auto source = std::string(source_path);
    auto index = source.find_last_of('.');
    auto path = source.substr(0, index + 1) + "sksl";

    std::ifstream stream(path);
    std::stringstream buffer;
    buffer << stream.rdbuf();

    return std::move(buffer).str();
}

std::string GetSkSLProgramKind(const std::string& content) {
    std::regex re(
        R"(^[ \t]*\/\/[ /\t]*kind[ \t]*[:=][ \t]*(shader|colorfilter|blender|meshfrag|meshvert))",
        std::regex_constants::ECMAScript
    );
    std::cmatch match;
    std::regex_search(content.data(), match, re);
    return match.str(1);
}

static std::string GetName(const Token::Value& value) {
    return std::visit(
        Overloaded {
            [](const SkSL::Extension* value) { return std::string(value->name()); },
            [](const SkSL::FunctionDeclaration* value) { return std::string(value->name()); },
            [](const SkSL::Variable* value) { return std::string(value->name()); },
            [](const SkSL::Type* value) { return std::string(value->name()); },
            [](const SkSL::Field* value) { return std::string(value->fName); },
            [](const SkSL::FieldSymbol* value) { return std::string(value->name()); },
            [](const SkSL::Literal* value) { return value->description(SkSL::OperatorPrecedence::kExpression); },
            [](const SkSL::Setting* value) { return std::string(value->name()); },
            [](const SkSL::Swizzle* value) { return SkSL::Swizzle::MaskString(value->components()); },
            [](const SkSL::ChildCall* /*value*/) { return std::string("eval"); },
        },
        value
    );
}

void ExpectTokens(const std::vector<Token>& actual, const std::vector<ExpectedToken>& expected) {
    auto i1 = actual.begin();
    auto i2 = expected.begin();
    for (; i1 != actual.end() && i2 != expected.end(); ++i1, ++i2) {
        ASSERT_EQ(i1->range, i2->range);
        ASSERT_EQ(i1->is_reference, i2->is_reference);
        ASSERT_EQ(GetName(i1->value), i2->name);
    }
    ASSERT_EQ(actual.size(), expected.size());
}

std::string ToString(const std::vector<Token>& tokens) {
    std::stringstream ss;
    for (const auto& token : tokens) {
        ss << "{{" << token.range.start << ", " << token.range.end << "}, " << (token.is_reference ? "true" : "false")
           << ", \"" << GetName(token.value) << "\"},\n";
    }
    return std::move(ss).str();
}
