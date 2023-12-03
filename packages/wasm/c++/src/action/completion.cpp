#include "action/completion.h"

#include <regex>

#include "data.h"

CompletionResult Completion(Modules* modules, const CompletionParams& params) {
    CompletionResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    std::regex re(R"(^[ \t]*\/\/[ /\t]*kind[ \t]*[:=])", std::regex_constants::ECMAScript);
    std::cmatch match;
    const auto* begin = iter->second.GetContent().begin();
    if (!std::regex_match(begin, begin + params.position, match, re)) {
        return result;
    }

    result.items.push_back({.label = "shader", .kind = SkSLCompletion::Kind::kText});
    result.items.push_back({.label = "colorfilter", .kind = SkSLCompletion::Kind::kText});
    result.items.push_back({.label = "blender", .kind = SkSLCompletion::Kind::kText});
    result.items.push_back({.label = "meshfrag", .kind = SkSLCompletion::Kind::kText});
    result.items.push_back({.label = "meshvert", .kind = SkSLCompletion::Kind::kText});
    return result;
}
