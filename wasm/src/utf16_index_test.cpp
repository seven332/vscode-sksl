#include "utf16_index.h"

#include <gtest/gtest.h>

#include <string_view>

TEST(UTF16IndexTest, Index) {
    std::string_view str = "\x61\x62\x63\xE4\xBD\xA0\xE5\xA5\xBD";
    UTF16Index index(str);

    EXPECT_EQ(index.ToUTF8(0), 0);
    EXPECT_EQ(index.ToUTF8(1), 1);
    EXPECT_EQ(index.ToUTF8(2), 2);
    EXPECT_EQ(index.ToUTF8(3), 3);
    EXPECT_EQ(index.ToUTF8(4), 6);
    EXPECT_EQ(index.ToUTF8(5), 9);
    EXPECT_EQ(index.ToUTF8(6), 9);

    EXPECT_EQ(index.ToUTF16(0), 0);
    EXPECT_EQ(index.ToUTF16(1), 1);
    EXPECT_EQ(index.ToUTF16(2), 2);
    EXPECT_EQ(index.ToUTF16(3), 3);
    EXPECT_EQ(index.ToUTF16(4), 4);
    EXPECT_EQ(index.ToUTF16(5), 4);
    EXPECT_EQ(index.ToUTF16(6), 4);
    EXPECT_EQ(index.ToUTF16(7), 5);
    EXPECT_EQ(index.ToUTF16(8), 5);
    EXPECT_EQ(index.ToUTF16(9), 5);
    EXPECT_EQ(index.ToUTF16(10), 5);
}
