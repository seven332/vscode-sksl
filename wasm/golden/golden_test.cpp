#include "golden_test.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

std::string ReadSkSL(const char* source_path) {
    auto source = std::string(source_path);
    auto index = source.find_last_of('.');
    auto path = source.substr(0, index + 1) + "sksl";

    std::ifstream stream(path);
    std::stringstream buffer;
    buffer << stream.rdbuf();

    return std::move(buffer).str();
}

std::string GetSkSLProgramKind(const std::string& content) {
    std::regex re(
        R"(^[ \t]*\/\/[ /\t]*kind[ \t]*[:=][ \t]*(shader|colorfilter|blender|meshfrag|meshvert))",
        std::regex_constants::ECMAScript
    );
    std::cmatch match;
    std::regex_search(content.data(), match, re);
    return match.str(1);
}
