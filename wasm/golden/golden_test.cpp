#include "golden_test.h"

#include <gtest/gtest.h>
#include <src/sksl/SkSLOperator.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include "data.h"
#include "module.h"
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

static std::string_view Substr(std::string_view content, const SkSLRange& range) {
    return content.substr(range.start, range.end - range.start);
}

void ExpectTokens(const Module& module, const std::vector<ExpectedToken>& expected) {
    auto i1 = module.tokens.begin();
    auto i2 = expected.begin();
    for (; i1 != module.tokens.end() && i2 != expected.end(); ++i1, ++i2) {
        ASSERT_EQ(i1->range, i2->range);
        ASSERT_EQ(i1->is_reference, i2->is_reference);
        ASSERT_EQ(Substr(module.content, i1->range), i2->name);
    }
    ASSERT_EQ(module.tokens.size(), expected.size());
}

std::string ToString(const Module& module) {
    std::stringstream ss;
    for (const auto& token : module.tokens) {
        ss << "{{" << token.range.start << ", " << token.range.end << "}, " << (token.is_reference ? "true" : "false")
           << ", \"" << Substr(module.content, token.range) << "\"},\n";
    }
    return std::move(ss).str();
}
