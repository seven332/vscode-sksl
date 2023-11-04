#include <gtest/gtest.h>

#include "action/close.h"
#include "action/update.h"
#include "golden_test.h"
#include "module.h"

TEST(GoldenTest, KawaseBlurRt) {
    Modules modules;
    const auto* file = "kawase_blur_rt.sksl";
    auto content = ReadSkSL(__FILE__);
    auto kind = GetSkSLProgramKind(content);
    auto update_result = Update(&modules, {.file = file, .content = content, .kind = kind});
    EXPECT_TRUE(update_result.succeed);
    // NOLINTBEGIN
    ExpectTokens(
        modules.find(file)->second,
        {
            {{23, 29},   true,  "shader"         },
            {{30, 37},   false, "in_blur"        },
            {{47, 53},   true,  "shader"         },
            {{54, 65},   false, "in_original"    },
            {{75, 80},   true,  "float"          },
            {{81, 96},   false, "in_inverseScale"},
            {{106, 111}, true,  "float"          },
            {{112, 118}, false, "in_mix"         },
            {{127, 131}, false, "main"           },
            {{139, 141}, false, "xy"             },
            {{149, 155}, true,  "float2"         },
            {{156, 165}, false, "scaled_xy"      },
            {{168, 174}, true,  "float2"         },
            {{175, 177}, true,  "xy"             },
            {{178, 179}, true,  "x"              },
            {{182, 197}, true,  "in_inverseScale"},
            {{199, 201}, true,  "xy"             },
            {{202, 203}, true,  "y"              },
            {{206, 221}, true,  "in_inverseScale"},
            {{229, 234}, true,  "half4"          },
            {{235, 242}, false, "blurred"        },
            {{245, 252}, true,  "in_blur"        },
            {{253, 257}, true,  "eval"           },
            {{258, 267}, true,  "scaled_xy"      },
            {{274, 279}, true,  "half4"          },
            {{280, 291}, false, "composition"    },
            {{294, 305}, true,  "in_original"    },
            {{306, 310}, true,  "eval"           },
            {{311, 313}, true,  "xy"             },
            {{327, 330}, true,  "mix"            },
            {{331, 342}, true,  "composition"    },
            {{344, 351}, true,  "blurred"        },
            {{353, 359}, true,  "in_mix"         },
    }
    );
    // NOLINTEND
    auto close_result = Close(&modules, {.file = file});
    EXPECT_TRUE(close_result.succeed);
}
