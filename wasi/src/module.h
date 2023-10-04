#pragma once

#include <src/sksl/SkSLCompiler.h>

#include <memory>
#include <string>
#include <unordered_map>

struct Module {
    std::string content;
    std::unique_ptr<SkSL::Module> module;
};

using Modules = std::unordered_map<std::string, Module>;
