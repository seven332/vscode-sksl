#pragma once

#include <src/sksl/ir/SkSLProgram.h>

#include <memory>
#include <string>
#include <unordered_map>

struct Module {
    std::string_view content;
    std::unique_ptr<SkSL::Program> program;
};

using Modules = std::unordered_map<std::string, Module>;
