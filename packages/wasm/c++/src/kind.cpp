#include "kind.h"

#include <cstdint>
#include <regex>
#include <string>

#include "hash.h"

std::optional<Kind> GetKind(const std::string& content) {
    std::regex re(R"(^[ \t]*\/\/[ /\t]*kind[ \t]*[:=][ \t]*(\w+))", std::regex_constants::ECMAScript);
    std::cmatch match;
    if (!std::regex_search(content.data(), match, re)) {
        return std::nullopt;
    }
    return Kind {
        .str = match.str(1),
        .position = static_cast<std::uint32_t>(match.position(1)),
        .length = static_cast<std::uint32_t>(match.length(1)),
    };
}

std::optional<SkSL::ProgramKind> ToSkSLProgramKind(const std::string& kind) {
    switch (Hash(kind.c_str())) {
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
        return std::nullopt;
    }
}
