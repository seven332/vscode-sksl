#include <gtest/gtest.h>

#include "action/close.h"
#include "action/update.h"
#include "golden_test.h"
#include "module.h"

TEST(GoldenTest, DestColor) {
    Modules modules;
    const auto* file = "destcolor.sksl";
    auto content = ReadSkSL(__FILE__);
    auto update_result = Update(&modules, {.file = file, .content = content});
    EXPECT_TRUE(update_result.succeed);
    // NOLINTBEGIN
    ExpectTokens(
        modules.find(file)->second,
        {
            {{22, 26}, false, "main" },
            {{33, 36}, false, "src"  },
            {{44, 47}, false, "dst"  },
            {{63, 68}, true,  "half4"},
            {{69, 70}, false, "1"    },
            {{74, 77}, true,  "dst"  },
    }
    );
    // NOLINTEND
    auto close_result = Close(&modules, {.file = file});
    EXPECT_TRUE(close_result.succeed);
}
