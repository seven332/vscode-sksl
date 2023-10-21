#pragma once

#include <src/sksl/ir/SkSLProgram.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "token.h"

struct Module {
    std::string_view content;
    std::unique_ptr<SkSL::Program> program;
    std::vector<Token> tokens;
};

using Modules = std::unordered_map<std::string, Module>;
