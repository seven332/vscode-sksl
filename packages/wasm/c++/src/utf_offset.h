#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "simple_codec.h"

struct UTFOffset {
    int utf8_offset;
    int utf16_offset;

    [[nodiscard]] bool operator==(const UTFOffset& other) const = default;

    friend void Write(std::vector<std::byte>* bytes, const UTFOffset& value) {
        Write(bytes, value.utf8_offset);
        Write(bytes, value.utf16_offset);
    }
};

std::vector<UTFOffset> ToUTFOffsets(std::string_view text);
