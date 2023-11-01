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
    // NOLINTBEGIN
    ExpectTokens(
        modules.find(file)->second,
        {
            {{23, 29},   true,  "shader"    },
            {{30, 38},   false, "srcImage"  },
            {{48, 54},   true,  "shader"    },
            {{55, 63},   false, "dstImage"  },
            {{73, 80},   true,  "blender"   },
            {{81, 91},   false, "arithBlend"},
            {{99, 103},  false, "main"      },
            {{111, 113}, false, "xy"        },
            {{128, 138}, true,  "arithBlend"},
            {{139, 143}, true,  "eval"      },
            {{144, 152}, true,  "srcImage"  },
            {{153, 157}, true,  "eval"      },
            {{158, 160}, true,  "xy"        },
            {{163, 171}, true,  "dstImage"  },
            {{172, 176}, true,  "eval"      },
            {{177, 179}, true,  "xy"        },
    }
    );
    // NOLINTEND
    auto close_result = Close(&modules, {.file = file});
    EXPECT_TRUE(close_result.succeed);
}
