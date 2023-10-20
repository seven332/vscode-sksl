#include "action/update.h"

#include <src/sksl/SkSLCompiler.h>
#include <src/sksl/SkSLErrorReporter.h>
#include <src/sksl/SkSLModuleLoader.h>
#include <src/sksl/SkSLParser.h>
#include <src/sksl/SkSLProgramKind.h>
#include <src/sksl/SkSLProgramSettings.h>
#include <src/sksl/SkSLUtil.h>

#include <iostream>
#include <string>

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

static std::unique_ptr<SkSL::Program> CompileProgram(
    SkSL::Compiler* compiler,
    SkSL::ProgramKind kind,
    const char* module_name,
    std::string module_source,
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
    auto program = parser.program();
    if (error_reporter && (!error_reporter->GetErrors().empty() || !program)) {
        std::cerr << "Abort: failed to complied " << module_name << std::endl;
        for (const auto& error : error_reporter->GetErrors()) {
            std::cerr << error.message << std::endl;
        }
        std::abort();
    }
    return program;
}

UpdateResult Update(Modules* modules, UpdateParams params) {
    if (params.content.size() <= sizeof(std::string)) {
        // Avoid small string optimization
        return {.succeed = false};
    }

    SkSL::Compiler compiler(SkSL::ShaderCapsFactory::Standalone());

    SkSLErrorReporter error_reporter;
    compiler.context().fErrors = &error_reporter;

    auto kind = ToProgramKind(params.kind);
    std::string_view content = params.content;
    auto program = CompileProgram(&compiler, kind, params.file.c_str(), std::move(params.content), nullptr);

    UpdateResult result;
    error_reporter.FetchErrors(&result.errors);
    result.succeed = program != nullptr;
    if (result.succeed) {
        (*modules)[std::move(params.file)] = {
            .content = content,
            .program = std::move(program),
        };
    }

    return result;
}
