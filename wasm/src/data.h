#pragma once

#include <src/sksl/SkSLPosition.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <ostream>
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

    [[nodiscard]] bool Intersects(const SkSLRange& other) const {
        return std::max(start, other.start) < std::min(end, other.end);
    }

    void Offset(int offset) {
        start += offset;
        end += offset;
    }

    void Join(const SkSLRange& other) {
        start = std::min(start, other.start);
        end = std::max(end, other.end);
    }

    [[nodiscard]] bool operator==(const SkSLRange& other) const = default;

    friend std::ostream& operator<<(std::ostream& os, const SkSLRange& range) {
        os << range.start << ", " << range.end;
        return os;
    }

    friend std::size_t Read(std::span<std::byte> bytes, std::size_t offset, SkSLRange* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, reinterpret_cast<int*>(&value->start));
        read += Read(bytes, offset + read, reinterpret_cast<int*>(&value->end));
        return read;
    }

    friend void Write(std::vector<std::byte>* bytes, const SkSLRange& value) {
        Write(bytes, static_cast<int>(value.start));
        Write(bytes, static_cast<int>(value.end));
    }

 private:
    static constexpr std::uint32_t kMax = std::numeric_limits<std::uint32_t>::max();
};

struct SkSLDiagnostic {
    enum class Severity : int {
        kError = 1,
        kWarning = 2,
        kInformation = 3,
        kHint = 4,
    };

    std::string message;
    SkSLRange range;
    Severity severity;

    friend void Write(std::vector<std::byte>* bytes, const SkSLDiagnostic& value) {
        Write(bytes, value.message);
        Write(bytes, value.range);
        Write(bytes, static_cast<int>(value.severity));
    }
};

struct SkSLSymbol {
    enum class Kind : int {
        kVariable = 0,
        kFunction = 1,
        kField = 2,
        kStruct = 3,
        kInterface = 4,
    };

    std::string name;
    std::string detail;
    Kind kind;
    SkSLRange range;
    SkSLRange selection_range;
    std::vector<SkSLSymbol> children;

    friend void Write(std::vector<std::byte>* bytes, const SkSLSymbol& value) {
        Write(bytes, value.name);
        Write(bytes, value.detail);
        Write(bytes, static_cast<int>(value.kind));
        Write(bytes, value.range);
        Write(bytes, value.selection_range);
        Write(bytes, value.children);
    }
};

struct SkSLToken {
    SkSLRange range;
    std::uint32_t token_type = 0;
    std::uint32_t token_modifiers = 0;

    friend void Write(std::vector<std::byte>* bytes, const SkSLToken& value) {
        Write(bytes, value.range);
        Write(bytes, static_cast<int>(value.token_type));
        Write(bytes, static_cast<int>(value.token_modifiers));
    }
};
