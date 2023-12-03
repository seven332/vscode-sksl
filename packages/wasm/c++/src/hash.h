#pragma once

#include <cstdint>

constexpr std::uint32_t Hash(const char* str) {
    constexpr std::uint32_t kInitialValue = 17;
    constexpr std::uint32_t kMultiplierValue = 37;
    std::uint32_t hash = kInitialValue;
    while (*str) {
        hash = hash * kMultiplierValue + *str;
        ++str;
    }
    return hash;
}
