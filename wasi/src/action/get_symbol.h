#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "data.h"
#include "modules.h"

struct GetSymbolParams {
    std::string file;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetSymbolParams, file)
};

struct GetSymbolResult {
    std::vector<SkSLSymbol> symbols;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetSymbolResult, symbols)
};

GetSymbolResult GetSymbol(Modules* modules, const GetSymbolParams& params);
