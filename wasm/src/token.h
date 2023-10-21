#pragma once

#include <src/sksl/SkSLPosition.h>
#include <src/sksl/ir/SkSLChildCall.h>
#include <src/sksl/ir/SkSLExpression.h>
#include <src/sksl/ir/SkSLExtension.h>
#include <src/sksl/ir/SkSLFieldSymbol.h>
#include <src/sksl/ir/SkSLFunctionDeclaration.h>
#include <src/sksl/ir/SkSLLiteral.h>
#include <src/sksl/ir/SkSLSetting.h>
#include <src/sksl/ir/SkSLSwizzle.h>
#include <src/sksl/ir/SkSLType.h>
#include <src/sksl/ir/SkSLVariable.h>

#include <variant>

#include "data.h"

struct Token {
    using Value = std::variant<
        const SkSL::Extension*,
        const SkSL::FunctionDeclaration*,
        const SkSL::Variable*,
        const SkSL::Type*,
        const SkSL::Field*,
        const SkSL::FieldSymbol*,
        const SkSL::Literal*,
        const SkSL::Setting*,
        const SkSL::Swizzle*,   // components
        const SkSL::ChildCall*  // eval
        >;

    Value value;
    SkSLRange range;
    bool is_reference = false;
};
