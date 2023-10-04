#pragma once

#include <src/sksl/SkSLPosition.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <nlohmann/json.hpp>
#include <string>

struct SkSLRange {
    std::uint32_t start;
    std::uint32_t end;

    SkSLRange() : start(kMax), end(kMax) {}
    SkSLRange(std::uint32_t start, std::uint32_t end) : start(start), end(end) {}
    SkSLRange(const SkSL::Position& position) : start(position.startOffset()), end(position.endOffset()) {}

    [[nodiscard]] bool IsValid() const {
        return start != kMax && end != kMax;
    }

    void Offset(int offset) {
        start += offset;
        end += offset;
    }

    void Join(const SkSLRange& other) {
        start = std::min(start, other.start);
        end = std::max(end, other.end);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SkSLRange, start, end)

 private:
    static constexpr std::uint32_t kMax = std::numeric_limits<std::uint32_t>::max();
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
