#include "action/get_symbol.h"

#include <src/sksl/ir/SkSLExtension.h>
#include <src/sksl/ir/SkSLFunctionDefinition.h>
#include <src/sksl/ir/SkSLFunctionPrototype.h>
#include <src/sksl/ir/SkSLInterfaceBlock.h>
#include <src/sksl/ir/SkSLModifiersDeclaration.h>
#include <src/sksl/ir/SkSLStructDefinition.h>
#include <src/sksl/ir/SkSLVarDeclarations.h>

#include <iostream>
#include <string_view>

#include "data.h"
#include "lexer.h"

static SkSLRange Find(const std::string& content, const SkSLRange& range, std::string_view name) {
    auto text = std::string_view(content.data() + range.start, content.data() + range.end);
    auto result = FindIdentifier(text, name);
    if (result.IsValid()) {
        result.Offset(static_cast<int>(range.start));
        return result;
    } else {
        return range;
    }
}

static SkSLSymbol Parse(const SkSL::FunctionDefinition& element, const std::string& content) {
    auto range = SkSLRange(element.position());
    range.Join(element.declaration().position());
    range.Join(element.body()->position());

    auto name = element.declaration().name();
    auto selection_range = Find(content, element.declaration().position(), name);

    return {
        .name = std::string(name),
        .kind = "function",
        .range = range,
        .selectionRange = selection_range,
    };
}

static SkSLSymbol Parse(const SkSL::GlobalVarDeclaration& element, const std::string& content) {
    auto range = SkSLRange(element.position());
    range.Join(element.declaration()->position());
    range.Join(element.varDeclaration().position());
    range.Join(element.varDeclaration().var()->position());

    auto name = element.varDeclaration().var()->name();
    auto selection_range = Find(content, element.varDeclaration().var()->position(), name);

    return {
        .name = std::string(name),
        .kind = "variable",
        .range = range,
        .selectionRange = selection_range,
    };
}

static SkSLSymbol Parse(const SkSL::StructDefinition& element, const std::string& content) {
    auto range = SkSLRange(element.position());
    range.Join(element.type().position());

    auto name = element.type().name();
    auto selection_range = Find(content, element.type().position(), name);

    return {
        .name = std::string(name),
        .kind = "struct",
        .range = range,
        .selectionRange = selection_range,
    };
}

GetSymbolResult GetSymbol(Modules* modules, const GetSymbolParams& params) {
    GetSymbolResult result;

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
            result.symbols.push_back(Parse(function, iter->second.content));
            break;
        }
        case SkSL::ProgramElementKind::kFunctionPrototype: {
            auto& prototype = element->as<SkSL::FunctionPrototype>();
            std::cerr << "TODO: GetSymbol kFunctionPrototype " << std::endl;
            break;
        }
        case SkSL::ProgramElementKind::kGlobalVar: {
            auto& var = element->as<SkSL::GlobalVarDeclaration>();
            result.symbols.push_back(Parse(var, iter->second.content));
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
            result.symbols.push_back(Parse(struct_d, iter->second.content));
            break;
        }
        }
    }

    return result;
}
