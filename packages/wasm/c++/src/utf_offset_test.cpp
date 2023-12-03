#include "utf_offset.h"

#include <gtest/gtest.h>

#include <string_view>
#include <vector>

TEST(UTFOffsetTest, Index) {
    std::string_view str = "\x61\x62\x63\xE4\xBD\xA0\xE5\xA5\xBD";
    auto offsets = ToUTFOffsets(str);
    // NOLINTBEGIN
    std::vector<UTFOffset> expected {
        {0, 0},
        {1, 1},
        {2, 2},
        {3, 3},
        {6, 4},
        {9, 5},
    };
    // NOLINTEND
    EXPECT_EQ(offsets, expected);
}
