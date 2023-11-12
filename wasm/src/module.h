#pragma once

#include <src/sksl/ir/SkSLProgram.h>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "token.h"

struct Document {
    std::unique_ptr<SkSL::Program> program;
    std::vector<Token> tokens;
};

struct Module {
    std::string_view content;
    std::optional<Document> document;
};

using Modules = std::unordered_map<std::string, Module>;
