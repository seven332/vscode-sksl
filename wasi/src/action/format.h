#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "module.h"

struct FormatParams {
    std::string file;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FormatParams, file)
};

struct FormatResult {
    std::string newContent;  // NOLINT
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FormatResult, newContent)
};

FormatResult Format(Modules* modules, const FormatParams& params);
