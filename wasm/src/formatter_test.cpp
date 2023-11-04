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

TEST(FormatterTest, NewLine) {
    EXPECT_STREQ(Formatter().Format("{\n}").c_str(), "{\n}\n");
    EXPECT_STREQ(Formatter().Format("{\n\n}").c_str(), "{\n\n}\n");
    EXPECT_STREQ(Formatter().Format("{\n\n\n}").c_str(), "{\n\n}\n");

    EXPECT_STREQ(Formatter().Format("{\n}\n").c_str(), "{\n}\n");
    EXPECT_STREQ(Formatter().Format("{\n}\n\n").c_str(), "{\n}\n");
}

TEST(FormatterTest, Paren) {
    EXPECT_STREQ(Formatter().Format("(a+b)").c_str(), "(a + b)\n");
    EXPECT_STREQ(Formatter().Format("((a+b))").c_str(), "((a + b))\n");
    EXPECT_STREQ(Formatter().Format("a=(b)").c_str(), "a = (b)\n");
}

TEST(FormatterTest, brace) {
    EXPECT_STREQ(Formatter().Format("{}//TODO").c_str(), "{\n}  // TODO\n");
    EXPECT_STREQ(Formatter().Format("{};").c_str(), "{\n};\n");
    EXPECT_STREQ(Formatter().Format("{};//TODO").c_str(), "{\n};  // TODO\n");
    EXPECT_STREQ(
        Formatter().Format("if (a) {} else {}").c_str(),
        "if (a) {\n"
        "} else {\n"
        "}\n"
    );
}

TEST(FormatterTest, Swizzle) {
    EXPECT_STREQ(Formatter().Format("a.rgb1").c_str(), "a.rgb1\n");
    EXPECT_STREQ(Formatter().Format("a.r1ba").c_str(), "a.r1ba\n");
    EXPECT_STREQ(Formatter().Format("a.1gba").c_str(), "a.1gba\n");
    EXPECT_STREQ(Formatter().Format("a.1gb1").c_str(), "a.1gb1\n");
}

TEST(FormatterTest, Layout) {
    EXPECT_STREQ(
        Formatter().Format("layout (color) uniform vec4 in_color;").c_str(),
        "layout(color) uniform vec4 in_color;\n"
    );
}
