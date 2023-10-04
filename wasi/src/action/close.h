#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "module.h"

struct CloseParams {
    std::string file;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CloseParams, file)
};

struct CloseResult {
    bool succeed = false;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CloseResult, succeed)
};

CloseResult Close(Modules* modules, const CloseParams& params);
