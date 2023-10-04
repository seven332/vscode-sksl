#pragma once

#include <src/sksl/SkSLCompiler.h>

#include <memory>
#include <string>
#include <unordered_map>

using Modules = std::unordered_map<std::string, std::unique_ptr<SkSL::Module>>;
