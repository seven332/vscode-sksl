#include "formatter.h"

#include <gtest/gtest.h>

TEST(FormatterTest, Operator) {
    EXPECT_STREQ(Formatter().Format("++a").c_str(), "++a\n");
    EXPECT_STREQ(Formatter().Format("a++").c_str(), "a++\n");
    EXPECT_STREQ(Formatter().Format("--a").c_str(), "--a\n");
    EXPECT_STREQ(Formatter().Format("a--").c_str(), "a--\n");

    EXPECT_STREQ(Formatter().Format("+a").c_str(), "+a\n");
    EXPECT_STREQ(Formatter().Format("a+b").c_str(), "a + b\n");
    EXPECT_STREQ(Formatter().Format("-a").c_str(), "-a\n");
    EXPECT_STREQ(Formatter().Format("a-b").c_str(), "a - b\n");

    EXPECT_STREQ(Formatter().Format("a*b").c_str(), "a * b\n");
    EXPECT_STREQ(Formatter().Format("a/b").c_str(), "a / b\n");
    EXPECT_STREQ(Formatter().Format("a%b").c_str(), "a % b\n");
    EXPECT_STREQ(Formatter().Format("a<<b").c_str(), "a << b\n");
    EXPECT_STREQ(Formatter().Format("a>>b").c_str(), "a >> b\n");
    EXPECT_STREQ(Formatter().Format("a|b").c_str(), "a | b\n");
    EXPECT_STREQ(Formatter().Format("a^b").c_str(), "a ^ b\n");
    EXPECT_STREQ(Formatter().Format("a&b").c_str(), "a & b\n");

    EXPECT_STREQ(Formatter().Format("~a").c_str(), "~a\n");

    EXPECT_STREQ(Formatter().Format("a||b").c_str(), "a || b\n");
    EXPECT_STREQ(Formatter().Format("a^^b").c_str(), "a ^^ b\n");
    EXPECT_STREQ(Formatter().Format("a&&b").c_str(), "a && b\n");

    EXPECT_STREQ(Formatter().Format("!a").c_str(), "!a\n");

    EXPECT_STREQ(Formatter().Format("a?b:c").c_str(), "a ? b : c\n");

    EXPECT_STREQ(Formatter().Format("a=b").c_str(), "a = b\n");
    EXPECT_STREQ(Formatter().Format("a==b").c_str(), "a == b\n");
    EXPECT_STREQ(Formatter().Format("a!=b").c_str(), "a != b\n");
    EXPECT_STREQ(Formatter().Format("a>b").c_str(), "a > b\n");
    EXPECT_STREQ(Formatter().Format("a<b").c_str(), "a < b\n");
    EXPECT_STREQ(Formatter().Format("a>=b").c_str(), "a >= b\n");
    EXPECT_STREQ(Formatter().Format("a<=b").c_str(), "a <= b\n");
    EXPECT_STREQ(Formatter().Format("a+=b").c_str(), "a += b\n");
    EXPECT_STREQ(Formatter().Format("a-=b").c_str(), "a -= b\n");
    EXPECT_STREQ(Formatter().Format("a*=b").c_str(), "a *= b\n");
    EXPECT_STREQ(Formatter().Format("a/=b").c_str(), "a /= b\n");
    EXPECT_STREQ(Formatter().Format("a%=b").c_str(), "a %= b\n");
    EXPECT_STREQ(Formatter().Format("a<<=b").c_str(), "a <<= b\n");
    EXPECT_STREQ(Formatter().Format("a>>=b").c_str(), "a >>= b\n");
    EXPECT_STREQ(Formatter().Format("a|=b").c_str(), "a |= b\n");
    EXPECT_STREQ(Formatter().Format("a^=b").c_str(), "a ^= b\n");
    EXPECT_STREQ(Formatter().Format("a&=b").c_str(), "a &= b\n");
}

TEST(FormatterTest, Comment) {
    EXPECT_STREQ(Formatter().Format("a;//TODO").c_str(), "a;  // TODO\n");
    EXPECT_STREQ(Formatter().Format("a;// TODO").c_str(), "a;  // TODO\n");
    EXPECT_STREQ(Formatter().Format("a;//  TODO").c_str(), "a;  // TODO\n");
    EXPECT_STREQ(Formatter().Format("a;/*TODO*/").c_str(), "a; /*TODO*/\n");
}
