#include "utf_offset.h"

#include <src/base/SkUTF.h>

#include <string_view>
#include <vector>

std::vector<UTFOffset> ToUTFOffsets(std::string_view text) {
    std::vector<UTFOffset> offsets;
    offsets.push_back(UTFOffset {0, 0});

    int utf16_offset = 0;
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

        auto utf8_offset = static_cast<int>(src - text.data());
        utf16_offset += static_cast<int>(count);
        offsets.push_back(UTFOffset {utf8_offset, utf16_offset});
    }

    return offsets;
}
