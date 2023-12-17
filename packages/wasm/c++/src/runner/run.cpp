#include "runner/run.h"

#include <include/core/SkStream.h>
#include <src/core/SkRasterPipeline.h>
#include <src/sksl/SkSLCompiler.h>
#include <src/sksl/SkSLParser.h>
#include <src/sksl/SkSLProgramSettings.h>
#include <src/sksl/SkSLUtil.h>
#include <src/sksl/codegen/SkSLRasterPipelineBuilder.h>
#include <src/sksl/codegen/SkSLRasterPipelineCodeGenerator.h>
#include <src/sksl/ir/SkSLFunctionDeclaration.h>
#include <src/sksl/ir/SkSLProgram.h>
#include <src/sksl/tracing/SkSLDebugTracePlayer.h>
#include <src/sksl/tracing/SkSLDebugTracePriv.h>

#include <string>

#include "data.h"
#include "kind.h"

static std::unique_ptr<SkSL::Program> CompileProgram(std::string source) {
    auto kind_result = GetKind(source);
    if (!kind_result) {
        return nullptr;
    }

    auto kind = ToSkSLProgramKind(kind_result->str);
    if (!kind) {
        return nullptr;
    }

    SkSL::Compiler compiler(SkSL::ShaderCapsFactory::Standalone());
    SkSL::ProgramSettings settings;
    settings.fUseMemoryPool = false;
    settings.fOptimize = false;
    settings.fForceNoInline = true;
    if (SkSL::ProgramConfig::IsRuntimeEffect(*kind)) {
        settings.fAllowNarrowingConversions = true;
    }
    SkSL::Parser parser(&compiler, settings, *kind, std::move(source));
    return parser.program();
}

RunResult Run(RunParams params) {
    RunResult result;

    auto program = CompileProgram(std::move(params.source));
    if (!program) {
        return result;
    }

    const auto* function = program->getFunction("main");
    if (!function) {
        return result;
    }

    auto trace = sk_make_sp<SkSL::DebugTracePriv>();
    std::unique_ptr<SkSL::RP::Program> raster_program =
        SkSL::MakeRasterPipelineProgram(*program, *function->definition(), trace.get(), true);
    if (!raster_program) {
        return result;
    }

    static constexpr auto kFirstHeapAllocation = 4096;
    SkArenaAlloc alloc(kFirstHeapAllocation);
    SkRasterPipeline pipeline(&alloc);
    // TODO: values
    raster_program->appendStages(&pipeline, &alloc, nullptr, {});

    SkRGBA4f<SkAlphaType::kPremul_SkAlphaType> out {};
    SkRasterPipeline_MemoryCtx out_ctx = {&out, 0};
    pipeline.append(SkRasterPipelineOp::store_f32, &out_ctx);
    pipeline.run(0, 0, 1, 1);

    result.succeed = true;
    auto unpremul = out.unpremul();
    result.color = SkSLColor {
        .r = unpremul.fR,
        .g = unpremul.fG,
        .b = unpremul.fB,
        .a = unpremul.fA,
    };
    return result;
}
