#include "utf16_index.h"

#include <src/base/SkUTF.h>

#include <algorithm>
#include <array>
#include <cstdint>

UTF16Index::UTF16Index() {
    index_.emplace_back(0, 0);
}

UTF16Index::UTF16Index(std::string_view text) {
    index_.emplace_back(0, 0);

    std::uint16_t utf16_offset = 0;
    const auto* src = text.data();
    const auto* end_src = src + text.size();
    while (src < end_src) {
        SkUnichar uni = SkUTF::NextUTF8(&src, end_src);
        if (uni < 0) {
            break;
        }

        std::array<std::uint16_t, 2> utf16 {};
        size_t count = SkUTF::ToUTF16(uni, utf16.data());
        if (count == 0) {
            break;
        }

        auto utf8_offset = src - text.data();
        utf16_offset += count;
        index_.emplace_back(utf8_offset, utf16_offset);
    }
}

std::uint16_t UTF16Index::ToUTF16(std::uint16_t offset) const {
    auto iter = std::lower_bound(
        index_.begin(),
        index_.end(),
        offset,
        [](const std::pair<std::uint16_t, std::uint16_t>& element, std::uint16_t offset) {
            return element.first < offset;
        }
    );
    return iter == index_.end() ? index_.back().second : iter->second;
}

std::uint16_t UTF16Index::ToUTF8(std::uint16_t offset) const {
    auto iter = std::lower_bound(
        index_.begin(),
        index_.end(),
        offset,
        [](const std::pair<std::uint16_t, std::uint16_t>& element, std::uint16_t offset) {
            return element.second < offset;
        }
    );
    return iter == index_.end() ? index_.back().first : iter->first;
}
