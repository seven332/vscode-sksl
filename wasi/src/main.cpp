#include <src/sksl/SkSLCompiler.h>
#include <src/sksl/SkSLErrorReporter.h>
#include <src/sksl/SkSLModuleLoader.h>
#include <src/sksl/SkSLParser.h>
#include <src/sksl/SkSLPosition.h>
#include <src/sksl/SkSLProgramKind.h>
#include <src/sksl/SkSLProgramSettings.h>
#include <src/sksl/SkSLUtil.h>
#include <src/sksl/ir/SkSLExtension.h>
#include <src/sksl/ir/SkSLFunctionDefinition.h>
#include <src/sksl/ir/SkSLFunctionPrototype.h>
#include <src/sksl/ir/SkSLInterfaceBlock.h>
#include <src/sksl/ir/SkSLModifiersDeclaration.h>
#include <src/sksl/ir/SkSLStructDefinition.h>
#include <src/sksl/ir/SkSLSymbolTable.h>
#include <src/sksl/ir/SkSLVarDeclarations.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <src/sksl/generated/sksl_public.minified.sksl>
#include <src/sksl/generated/sksl_shared.minified.sksl>
#include <string>
#include <unordered_map>
#include <vector>

struct SkSLRange {
    std::uint32_t start;
    std::uint32_t end;

    SkSLRange() : start(0), end(0) {}
    SkSLRange(const SkSL::Position& position) : start(position.startOffset()), end(position.endOffset()) {}

    void Join(const SkSLRange& other) {
        start = std::min(start, other.start);
        end = std::min(end, other.end);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SkSLRange, start, end)
};

struct SkSLError {
    std::string message;
    SkSLRange range;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SkSLError, message, range)
};

struct SkSLSymbol {
    std::string name;
    std::string kind;
    SkSLRange range;
    SkSLRange selectionRange;  // NOLINT
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SkSLSymbol, name, kind, range, selectionRange)
};

static std::unordered_map<std::string, std::unique_ptr<SkSL::Module>> modules;

static constexpr std::uint32_t Hash(const char* str) {
    constexpr std::uint32_t kInitialValue = 17;
    constexpr std::uint32_t kMultiplierValue = 37;
    std::uint32_t hash = kInitialValue;
    while (*str) {
        hash = hash * kMultiplierValue + *str;
        ++str;
    }
    return hash;
}

#pragma mark - Update

struct UpdateParams {
    std::string file;
    std::string content;
    std::string kind;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UpdateParams, file, content, kind)
};

struct UpdateResult {
    bool succeed = false;
    std::vector<SkSLError> errors;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UpdateResult, succeed, errors)
};

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
    case Hash("mesh-vert"):
        return SkSL::ProgramKind::kMeshVertex;
    case Hash("mesh-frag"):
        return SkSL::ProgramKind::kMeshFragment;
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

static void Update(const UpdateParams& params) {
    SkSL::Compiler compiler(SkSL::ShaderCapsFactory::Standalone());

    SkSLErrorReporter error_reporter;
    compiler.context().fErrors = &error_reporter;

    static const auto kBuiltinModule = CompileBuiltinModule();
    auto kind = ToProgramKind(params.kind);
    auto module = CompileModule(&compiler, kind, params.file.c_str(), params.content, kBuiltinModule.get(), nullptr);

    UpdateResult result;
    error_reporter.FetchErrors(&result.errors);
    result.succeed = module != nullptr;
    if (result.succeed) {
        modules[params.file] = std::move(module);
    }

    nlohmann::json j = result;
    std::cout << j.dump() << std::endl;
}

#pragma mark - Close

struct CloseParams {
    std::string file;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CloseParams, file)
};

struct CloseResult {
    bool succeed = false;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CloseResult, succeed)
};

static void Close(const CloseParams& params) {
    CloseResult result;

    auto iter = modules.find(params.file);
    if (iter == modules.end()) {
        result.succeed = false;
    } else {
        modules.erase(iter);
        result.succeed = true;
    }

    nlohmann::json j = result;
    std::cout << j.dump() << std::endl;
}

#pragma mark - GetSymbol

struct GetSymbolParams {
    std::string file;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetSymbolParams, file)
};

struct GetSymbolResult {
    std::vector<SkSLSymbol> symbols;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetSymbolResult, symbols)
};

static void GetSymbol(const GetSymbolParams& params) {
    GetSymbolResult result;

    auto iter = modules.find(params.file);
    if (iter != modules.end()) {
        for (const auto& element : iter->second->fElements) {
            switch (element->kind()) {
            case SkSL::ProgramElementKind::kExtension: {
                auto& extension = element->as<SkSL::Extension>();
                std::cerr << "TODO: GetSymbol kExtension " << std::endl;
                break;
            }
            case SkSL::ProgramElementKind::kFunction: {
                auto& function = element->as<SkSL::FunctionDefinition>();
                auto range = SkSLRange(function.position());
                range.Join(function.declaration().position());
                range.Join(function.body()->position());
                result.symbols.push_back(SkSLSymbol {
                    .name = std::string(function.declaration().name()),
                    .kind = "function",
                    .range = range,
                    .selectionRange = function.declaration().position(),
                });
                break;
            }
            case SkSL::ProgramElementKind::kFunctionPrototype: {
                auto& prototype = element->as<SkSL::FunctionPrototype>();
                std::cerr << "TODO: GetSymbol kFunctionPrototype " << std::endl;
                break;
            }
            case SkSL::ProgramElementKind::kGlobalVar: {
                auto& var = element->as<SkSL::GlobalVarDeclaration>();
                auto range = SkSLRange(var.position());
                range.Join(var.declaration()->position());
                range.Join(var.varDeclaration().position());
                range.Join(var.varDeclaration().var()->position());
                result.symbols.push_back(SkSLSymbol {
                    .name = std::string(var.varDeclaration().var()->name()),
                    .kind = "variable",
                    .range = range,
                    .selectionRange = var.varDeclaration().var()->position(),
                });
                break;
            }
            case SkSL::ProgramElementKind::kInterfaceBlock: {
                auto& interface = element->as<SkSL::InterfaceBlock>();
                std::cerr << "TODO: GetSymbol kInterfaceBlock " << std::endl;
                break;
            }
            case SkSL::ProgramElementKind::kModifiers: {
                auto& modifiers = element->as<SkSL::ModifiersDeclaration>();
                std::cerr << "TODO: GetSymbol kModifiers " << std::endl;
                break;
            }
            case SkSL::ProgramElementKind::kStructDefinition: {
                auto& struct_d = element->as<SkSL::StructDefinition>();
                auto range = SkSLRange(struct_d.position());
                range.Join(struct_d.type().position());
                result.symbols.push_back(SkSLSymbol {
                    .name = std::string(struct_d.type().name()),
                    .kind = "struct",
                    .range = range,
                    .selectionRange = struct_d.type().position(),
                });
                break;
            }
            }
        }
    }

    nlohmann::json j = result;
    std::cout << j.dump() << std::endl;
}

#pragma mark - Main

static std::string GetLine() {
    constexpr std::uint32_t kNextLine = 10;
    std::string result;
    while (true) {
        int c = std::cin.get();
        if (c == kNextLine || c == std::istream::traits_type::eof()) {
            return result;
        } else {
            result += static_cast<char>(c);
        }
    }
}

extern "C" {

#define CALL(Action)                    \
auto j = nlohmann::json::parse(params); \
auto params = j.get<Action##Params>();  \
Action(params);

int main(void) {
    while (true) {
        auto method = GetLine();
        auto params = GetLine();
        switch (Hash(method.data())) {
        case Hash("sksl/update"): {
            CALL(Update)
            break;
        }
        case Hash("sksl/close"): {
            CALL(Close)
            break;
        }
        case Hash("sksl/get-symbol"): {
            CALL(GetSymbol)
            break;
        }
        default:
            std::cerr << "Abort: invalid method: " << method << std::endl;
            std::abort();
        }
    }
    return 0;
}
}
