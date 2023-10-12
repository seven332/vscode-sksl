#include "simple_codec.h"

#include <gtest/gtest.h>

#include <vector>

TEST(SimpleCodecTest, Bool) {
    std::vector<std::byte> bytes;
    Write(&bytes, true);
    bool result = false;
    auto read = Read(bytes, 0, &result);
    EXPECT_EQ(result, true);
    EXPECT_EQ(read, bytes.size());
}

TEST(SimpleCodecTest, Int) {
    static constexpr int kValue = 123;
    std::vector<std::byte> bytes;
    Write(&bytes, kValue);
    int result = 0;
    auto read = Read(bytes, 0, &result);
    EXPECT_EQ(result, kValue);
    EXPECT_EQ(read, bytes.size());
}

TEST(SimpleCodecTest, String) {
    static const std::string kValue = "hello🌎𠮷";
    std::vector<std::byte> bytes;
    Write(&bytes, kValue);
    std::string result;
    auto read = Read(bytes, 0, &result);
    EXPECT_STREQ(result.c_str(), kValue.c_str());
    EXPECT_EQ(read, bytes.size());
}

TEST(SimpleCodecTest, Array) {
    static const std::vector<int> kValue = {1, 4, 5, 2};
    std::vector<std::byte> bytes;
    Write(&bytes, kValue);
    std::vector<int> result;
    auto read = Read(bytes, 0, &result);
    EXPECT_EQ(result, kValue);
    EXPECT_EQ(read, bytes.size());
}
