#pragma once

#include <src/sksl/SkSLLexer.h>

#include <functional>
#include <string_view>

#include "data.h"

SkSLRange FindIdentifier(std::string_view content, std::string_view name);

void TraverseTokens(std::string_view content, const std::function<void(const SkSL::Token& token)>& block);
