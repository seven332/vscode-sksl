#include <src/sksl/SkSLCompiler.h>
#include <src/sksl/SkSLErrorReporter.h>
#include <src/sksl/SkSLModuleLoader.h>
#include <src/sksl/SkSLProgramKind.h>
#include <src/sksl/SkSLUtil.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

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

struct SkSLError {
    std::string msg;
    std::uint32_t start;
    std::uint32_t end;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SkSLError, msg, start, end)
};

struct UpdateResult {
    std::vector<SkSLError> errors;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UpdateResult, errors)
};

class SkSLErrorReporter : public SkSL::ErrorReporter {
 public:
    void FetchErrors(std::vector<SkSLError>* errors) {
        *errors = std::move(errors_);
    }

 protected:
    void handleError(std::string_view msg, SkSL::Position position) override {
        errors_.push_back(
            {std::string(msg),
             static_cast<std::uint32_t>(position.startOffset()),
             static_cast<std::uint32_t>(position.endOffset())}
        );
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

static void Update(const UpdateParams& params) {
    SkSL::Compiler compiler(SkSL::ShaderCapsFactory::Standalone());

    SkSLErrorReporter error_reporter;
    compiler.context().fErrors = &error_reporter;

    auto kind = ToProgramKind(params.kind);
    compiler.compileModule(kind, params.file.c_str(), params.content, SkSL::ModuleLoader::Get().rootModule(), false);

    UpdateResult result;
    error_reporter.FetchErrors(&result.errors);
    nlohmann::json j = result;
    std::cout << j.dump() << std::endl;
}

#pragma mark - Close

struct CloseParams {
    std::string file;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CloseParams, file)
};

static void Close(const CloseParams& params) {
    // TODO:
    std::cout << "Close: " << params.file << std::endl;
}

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
        default:
            std::cerr << "Abort: invalid method: " << method << std::endl;
            std::abort();
        }
    }
    return 0;
}
}
