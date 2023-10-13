#pragma once

#include <src/sksl/SkSLPosition.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include "simple_codec.h"

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

    friend void Write(std::vector<std::byte>* bytes, const SkSLRange& value) {
        Write(bytes, static_cast<int>(value.start));
        Write(bytes, static_cast<int>(value.end));
    }

 private:
    static constexpr std::uint32_t kMax = std::numeric_limits<std::uint32_t>::max();
};

struct SkSLError {
    std::string message;
    SkSLRange range;

    friend void Write(std::vector<std::byte>* bytes, const SkSLError& value) {
        Write(bytes, value.message);
        Write(bytes, value.range);
    }
};

struct SkSLSymbol {
    std::string name;
    std::string detail;
    std::string kind;
    SkSLRange range;
    SkSLRange selectionRange;  // NOLINT
    std::vector<SkSLSymbol> children;

    friend void Write(std::vector<std::byte>* bytes, const SkSLSymbol& value) {
        Write(bytes, value.name);
        Write(bytes, value.detail);
        Write(bytes, value.kind);
        Write(bytes, value.range);
        Write(bytes, value.selectionRange);
        Write(bytes, value.children);
    }
};
