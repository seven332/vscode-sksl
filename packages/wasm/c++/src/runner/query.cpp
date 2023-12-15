#include "runner/query.h"

#include <src/core/SkRasterPipeline.h>
#include <src/sksl/SkSLCompiler.h>
#include <src/sksl/SkSLParser.h>
#include <src/sksl/SkSLProgramSettings.h>
#include <src/sksl/SkSLUtil.h>
#include <src/sksl/codegen/SkSLRasterPipelineBuilder.h>
#include <src/sksl/codegen/SkSLRasterPipelineCodeGenerator.h>
#include <src/sksl/ir/SkSLFunctionDeclaration.h>
#include <src/sksl/ir/SkSLIRNode.h>
#include <src/sksl/ir/SkSLProgram.h>
#include <src/sksl/ir/SkSLVarDeclarations.h>

#include <optional>

#include "kind.h"

static std::unique_ptr<SkSL::Program> CompileProgram(std::string source, SkSL::ProgramKind kind) {
    SkSL::Compiler compiler(SkSL::ShaderCapsFactory::Standalone());
    SkSL::ProgramSettings settings;
    settings.fUseMemoryPool = false;
    settings.fOptimize = false;
    settings.fForceNoInline = true;
    if (SkSL::ProgramConfig::IsRuntimeEffect(kind)) {
        settings.fAllowNarrowingConversions = true;
    }
    SkSL::Parser parser(&compiler, settings, kind, std::move(source));
    return parser.program();
}

QueryResult Query(QueryParams params) {
    QueryResult result;

    auto kind_result = GetKind(params.source);
    if (!kind_result) {
        return result;
    }

    auto kind = ToSkSLProgramKind(kind_result->str);
    if (!kind) {
        return result;
    }

    auto program = CompileProgram(std::move(params.source), *kind);
    if (!kind) {
        return result;
    }

    result.succeed = true;
    result.kind = kind_result->str;
    for (const auto& element : program->fOwnedElements) {
        if (element->kind() == SkSL::ProgramElementKind::kGlobalVar) {
            auto* var = element->as<SkSL::GlobalVarDeclaration>().varDeclaration().var();
            if (var->modifierFlags().isUniform()) {
                result.uniforms.push_back(SkSLUniform {var->type().abbreviatedName(), std::string(var->name())});
            }
        }
    }
    return result;
}
