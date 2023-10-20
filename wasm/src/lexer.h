#pragma once

#include <string>
#include <string_view>

#include "data.h"

SkSLRange FindIdentifier(std::string_view content, std::string_view name);
