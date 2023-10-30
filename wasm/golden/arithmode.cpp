#include <gtest/gtest.h>

#include "action/close.h"
#include "action/update.h"
#include "golden_test.h"
#include "module.h"

TEST(GoldenTest, ArithMode) {
    Modules modules;
    const auto* file = "arithmode.sksl";
    auto content = ReadSkSL(__FILE__);
    auto kind = GetSkSLProgramKind(content);
    auto update_result = Update(&modules, {.file = file, .content = content, .kind = kind});
    EXPECT_TRUE(update_result.succeed);
    auto close_result = Close(&modules, {.file = file});
    EXPECT_TRUE(close_result.succeed);
}
