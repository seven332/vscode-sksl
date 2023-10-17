#include "get_token.h"

#include <cstdint>
#include <iostream>
#include <string_view>

#include "data.h"
#include "src/sksl/ir/SkSLExtension.h"
#include "src/sksl/ir/SkSLFunctionDefinition.h"
#include "src/sksl/ir/SkSLFunctionPrototype.h"
#include "src/sksl/ir/SkSLInterfaceBlock.h"
#include "src/sksl/ir/SkSLModifiersDeclaration.h"
#include "src/sksl/ir/SkSLStructDefinition.h"
#include "src/sksl/ir/SkSLVarDeclarations.h"

enum TokenType : std::uint32_t {
    kClass,
    kInterface,
    kStruct,
    kParameter,
    kVariable,
    kProperty,
    kFunction,
};

enum TokenModifier : std::uint32_t {
    kReadonly = 1,
    kDefaultLibrary = 1,
};

static SkSLRange Find(std::string_view content, std::string_view text) {
    if (text.data() >= content.data() && text.data() + text.size() <= content.data() + content.size()) {
        std::uint32_t start = text.data() - content.data();
        std::uint32_t end = start + text.size();
        return SkSLRange(start, end);
    } else {
        return {};
    }
}

static void Parse(std::vector<SkSLToken>* tokens, const SkSL::FunctionDefinition& element, std::string_view content) {
    auto name = element.declaration().name();
    auto range = Find(content, name);
    if (range.IsValid()) {
        tokens->push_back(SkSLToken {.range = range, .tokenType = TokenType::kFunction, .tokenModifiers = 0});
    }
    // TODO: body
}

GetTokenResult GetToken(Modules* modules, const GetTokenParams& params) {
    GetTokenResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    for (const auto& element : iter->second.module->fElements) {
        switch (element->kind()) {
        case SkSL::ProgramElementKind::kExtension: {
            auto& extension = element->as<SkSL::Extension>();
            std::cerr << "TODO: GetSymbol kExtension " << std::endl;
            break;
        }
        case SkSL::ProgramElementKind::kFunction: {
            auto& function = element->as<SkSL::FunctionDefinition>();
            Parse(&result.tokens, function, iter->second.content);
            break;
        }
        case SkSL::ProgramElementKind::kFunctionPrototype: {
            auto& extension = element->as<SkSL::FunctionPrototype>();
            std::cerr << "TODO: GetSymbol kFunctionPrototype " << std::endl;
            break;
        }
        case SkSL::ProgramElementKind::kGlobalVar: {
            auto& extension = element->as<SkSL::GlobalVarDeclaration>();
            std::cerr << "TODO: GetSymbol kGlobalVar " << std::endl;
            break;
        }
        case SkSL::ProgramElementKind::kInterfaceBlock: {
            auto& extension = element->as<SkSL::InterfaceBlock>();
            std::cerr << "TODO: GetSymbol kInterfaceBlock " << std::endl;
            break;
        }
        case SkSL::ProgramElementKind::kModifiers: {
            auto& extension = element->as<SkSL::ModifiersDeclaration>();
            std::cerr << "TODO: GetSymbol kModifiers " << std::endl;
            break;
        }
        case SkSL::ProgramElementKind::kStructDefinition: {
            auto& extension = element->as<SkSL::StructDefinition>();
            std::cerr << "TODO: GetSymbol kModifiers " << std::endl;
            break;
        }
        }
    }

    return result;
}
