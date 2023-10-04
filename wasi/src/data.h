#pragma once

#include <src/sksl/SkSLPosition.h>

#include <algorithm>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

struct SkSLRange {
    std::uint32_t start;
    std::uint32_t end;

    SkSLRange() : start(0), end(0) {}
    SkSLRange(const SkSL::Position& position) : start(position.startOffset()), end(position.endOffset()) {}

    void Join(const SkSLRange& other) {
        start = std::min(start, other.start);
        end = std::min(end, other.end);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SkSLRange, start, end)
};

struct SkSLError {
    std::string message;
    SkSLRange range;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SkSLError, message, range)
};

struct SkSLSymbol {
    std::string name;
    std::string kind;
    SkSLRange range;
    SkSLRange selectionRange;  // NOLINT

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SkSLSymbol, name, kind, range, selectionRange)
};
