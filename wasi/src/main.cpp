#include <src/sksl/SkSLCompiler.h>
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
    auto kind = ToProgramKind(params.kind);
    compiler.compileModule(kind, params.file.c_str(), params.content, SkSL::ModuleLoader::Get().rootModule(), false);
    // TODO:
    std::cout << "Update: " << params.file << std::endl;
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
