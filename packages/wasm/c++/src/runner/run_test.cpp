
#include "runner/run.h"

#include <gtest/gtest.h>

TEST(RunTest, Run) {
    auto result = ::Run(RunParams {
        .source = ""
                  "// kind=shader\n"
                  "half4 main(float2 coord) {\n"
                  "    coord.x = 0.2;"
                  "    coord.y = 0.3;"
                  "    return float4(coord.x, coord.y, 0, 1);\n"
                  "}\n",
    });
    EXPECT_TRUE(result.succeed);
    EXPECT_EQ(result.color.r, 0.2F);
    EXPECT_EQ(result.color.g, 0.3F);
    EXPECT_EQ(result.color.b, 0.F);
    EXPECT_EQ(result.color.a, 1.F);
}
