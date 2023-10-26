#pragma once

#include <src/sksl/ir/SkSLProgram.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "token.h"
#include "utf16_index.h"

struct Module {
    std::string_view content;
    std::unique_ptr<SkSL::Program> program;
    std::vector<Token> tokens;
    UTF16Index utf16_index;
};

using Modules = std::unordered_map<std::string, Module>;
