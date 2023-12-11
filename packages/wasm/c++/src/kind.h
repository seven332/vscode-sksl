#pragma once

#include <src/sksl/SkSLProgramKind.h>

#include <cstdint>
#include <optional>
#include <string>

struct Kind {
    std::string str;
    std::uint32_t position;
    std::uint32_t length;
};

std::optional<Kind> GetKind(const std::string& content);

std::optional<SkSL::ProgramKind> ToSkSLProgramKind(const std::string& kind);
