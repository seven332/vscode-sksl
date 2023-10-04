#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "data.h"
#include "modules.h"

struct UpdateParams {
    std::string file;
    std::string content;
    std::string kind;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UpdateParams, file, content, kind)
};

struct UpdateResult {
    bool succeed = false;
    std::vector<SkSLError> errors;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UpdateResult, succeed, errors)
};

UpdateResult Update(Modules* modules, const UpdateParams& params);
