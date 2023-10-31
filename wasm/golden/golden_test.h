#pragma once

#include <string>

#include "data.h"
#include "token.h"

std::string ReadSkSL(const char* source_path);

std::string GetSkSLProgramKind(const std::string& content);

struct ExpectedToken {
    SkSLRange range;
    bool is_reference = false;
    std::string name;
};

void ExpectTokens(const std::vector<Token>& actual, const std::vector<ExpectedToken>& expected);

std::string ToString(const std::vector<Token>& tokens);
