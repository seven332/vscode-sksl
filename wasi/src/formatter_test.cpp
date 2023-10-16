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