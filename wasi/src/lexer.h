#pragma once

#include <string>
#include <string_view>

#include "data.h"

SkSLRange FindIdentifier(std::string_view text, std::string_view name);
