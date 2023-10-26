#pragma once

#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>

class UTF16Index {
 public:
    UTF16Index();
    explicit UTF16Index(std::string_view text);

    [[nodiscard]] std::uint16_t ToUTF16(std::uint16_t offset) const;
    [[nodiscard]] std::uint16_t ToUTF8(std::uint16_t offset) const;

 private:
    // utf-8, utf-16
    std::vector<std::pair<std::uint16_t, std::uint16_t>> index_;
};
