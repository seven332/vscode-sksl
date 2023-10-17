#include "action/update.h"

#include <src/sksl/SkSLErrorReporter.h>
#include <src/sksl/SkSLModuleLoader.h>
#include <src/sksl/SkSLParser.h>
#include <src/sksl/SkSLProgramKind.h>
#include <src/sksl/SkSLProgramSettings.h>
#include <src/sksl/SkSLUtil.h>

#include <iostream>
#include <src/sksl/generated/sksl_public.minified.sksl>
#include <src/sksl/generated/sksl_shared.minified.sksl>

#include "hash.h"

class SkSLErrorReporter : public SkSL::ErrorReporter {
 public:
    [[nodiscard]] const std::vector<SkSLError>& GetErrors() const {
        return errors_;
    }

    void FetchErrors(std::vector<SkSLError>* errors) {
        *errors = std::move(errors_);
    }

 protected:
    void handleError(std::string_view msg, SkSL::Position position) override {
        errors_.push_back({
            .message = std::string(msg),
            .range = position,
        });
    }

 private:
    std::vector<SkSLError> errors_;
};

static SkSL::ProgramKind ToProgramKind(const std::string& kind) {
    switch (Hash(kind.data())) {
    case Hash("frag"):
        return SkSL::ProgramKind::kFragment;
    case Hash("vert"):
        return SkSL::ProgramKind::kVertex;
    case Hash("compute"):
        return SkSL::ProgramKind::kCompute;
    case Hash("shader"):
        return SkSL::ProgramKind::kRuntimeShader;
    case Hash("colorfilter"):
        return SkSL::ProgramKind::kRuntimeColorFilter;
    case Hash("blender"):
        return SkSL::ProgramKind::kRuntimeBlender;
    case Hash("meshfrag"):
        return SkSL::ProgramKind::kMeshFragment;
    case Hash("meshvert"):
        return SkSL::ProgramKind::kMeshVertex;
    default:
        std::cerr << "Abort: invalid program kind: " << kind << std::endl;
        std::abort();
    }
}

static std::unique_ptr<SkSL::Module> CompileModule(
    SkSL::Compiler* compiler,
    SkSL::ProgramKind kind,
    const char* module_name,
    std::string module_source,
    const SkSL::Module* parent,
    SkSLErrorReporter* error_reporter
) {
    SkSL::ProgramSettings settings;
    settings.fOptimize = false;
    settings.fRemoveDeadFunctions = false;
    settings.fRemoveDeadVariables = false;
    if (SkSL::ProgramConfig::IsRuntimeEffect(kind)) {
        settings.fAllowNarrowingConversions = true;
    }
    SkSL::Parser parser {compiler, settings, kind, std::move(module_source)};
    auto module = parser.moduleInheritingFrom(parent);
    if (error_reporter && (!error_reporter->GetErrors().empty() || !module)) {
        std::cerr << "Abort: failed to complied " << module_name << std::endl;
        for (const auto& error : error_reporter->GetErrors()) {
            std::cerr << error.message << std::endl;
        }
        std::abort();
    }
    return module;
}

static std::unique_ptr<SkSL::Module> CompileBuiltinModule() {
    SkSL::Compiler compiler(SkSL::ShaderCapsFactory::Standalone());

    SkSLErrorReporter error_reporter;
    compiler.context().fErrors = &error_reporter;

    auto shared_module = CompileModule(
        &compiler,
        SkSL::ProgramKind::kFragment,
        "sksl_shared.sksl",
        reinterpret_cast<const char*>(SKSL_MINIFIED_sksl_shared),
        SkSL::ModuleLoader::Get().rootModule(),
        &error_reporter
    );

    auto public_module = CompileModule(
        &compiler,
        SkSL::ProgramKind::kFragment,
        "sksl_public.sksl",
        reinterpret_cast<const char*>(SKSL_MINIFIED_sksl_public),
        shared_module.get(),
        &error_reporter
    );

    SkSL::ModuleLoader::Get().addPublicTypeAliases(public_module.get());

    return public_module;
}

UpdateResult Update(Modules* modules, UpdateParams params) {
    SkSL::Compiler compiler(SkSL::ShaderCapsFactory::Standalone());

    SkSLErrorReporter error_reporter;
    compiler.context().fErrors = &error_reporter;

    static const auto kBuiltinModule = CompileBuiltinModule();
    auto kind = ToProgramKind(params.kind);
    std::string_view content = params.content;
    auto module =
        CompileModule(&compiler, kind, params.file.c_str(), std::move(params.content), kBuiltinModule.get(), nullptr);

    UpdateResult result;
    error_reporter.FetchErrors(&result.errors);
    result.succeed = module != nullptr;
    if (result.succeed) {
        (*modules)[std::move(params.file)] = {
            .content = content,
            .module = std::move(module),
        };
    }

    return result;
}
