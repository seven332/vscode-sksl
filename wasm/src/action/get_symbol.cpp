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
#include <vector>

#include "data.h"
#include "lexer.h"

static SkSLRange Find(std::string_view content, const SkSLRange& range, std::string_view name) {
    auto text = content.substr(range.start, range.end - range.start);
    auto result = FindIdentifier(text, name);
    if (result.IsValid()) {
        result.Offset(static_cast<int>(range.start));
        return result;
    } else {
        return range;
    }
}

static std::vector<SkSLSymbol> ToFieldSymbols(const SkSL::Type& type, std::string_view content) {
    std::vector<SkSLSymbol> children;
    children.reserve(type.fields().size());
    for (const auto& field : type.fields()) {
        children.push_back({
            .name = std::string(field.fName),
            .detail = field.fType->description(),
            .kind = "field",
            .range = field.fPosition,
            .selectionRange = Find(content, field.fPosition, field.fName),
        });
    }
    return children;
}

static void Parse(std::vector<SkSLSymbol>* symbols, const SkSL::FunctionDefinition& element, std::string_view content) {
    auto range = SkSLRange(element.position());
    range.Join(element.declaration().position());
    range.Join(element.body()->position());

    auto name = element.declaration().name();
    auto selection_range = Find(content, element.declaration().position(), name);

    symbols->push_back({
        .name = std::string(name),
        .detail = element.declaration().description(),
        .kind = "function",
        .range = range,
        .selectionRange = selection_range,
    });
}

static void Parse(std::vector<SkSLSymbol>* symbols, const SkSL::FunctionPrototype& element, std::string_view content) {
    auto range = SkSLRange(element.position());
    range.Join(element.declaration().position());

    auto name = element.declaration().name();
    auto selection_range = Find(content, element.declaration().position(), name);

    symbols->push_back({
        .name = std::string(name),
        .detail = element.declaration().description(),
        .kind = "function",
        .range = range,
        .selectionRange = selection_range,
    });
}

static void
Parse(std::vector<SkSLSymbol>* symbols, const SkSL::GlobalVarDeclaration& element, std::string_view content) {
    auto range = SkSLRange(element.position());
    range.Join(element.varDeclaration().position());
    range.Join(element.varDeclaration().var()->position());

    auto name = element.varDeclaration().var()->name();
    auto selection_range = Find(content, element.varDeclaration().var()->position(), name);

    symbols->push_back({
        .name = std::string(name),
        .detail = element.varDeclaration().var()->type().description(),
        .kind = "variable",
        .range = range,
        .selectionRange = selection_range,
    });
}

static void Parse(std::vector<SkSLSymbol>* symbols, const SkSL::InterfaceBlock& element, std::string_view content) {
    {
        const auto& type = element.var()->type();
        auto name = type.name();
        auto range = SkSLRange(type.position());
        auto selection_range = Find(content, range, name);

        symbols->push_back({
            .name = std::string(name),
            .detail = "interface",
            .kind = "interface",
            .range = range,
            .selectionRange = selection_range,
            .children = ToFieldSymbols(element.var()->type(), content),
        });
    }

    {
        const auto& var = element.var();
        auto name = var->name();
        if (!name.empty()) {
            auto range = SkSLRange(var->position());
            auto selection_range = Find(content, range, name);

            symbols->push_back({
                .name = std::string(name),
                .detail = var->type().description(),
                .kind = "variable",
                .range = range,
                .selectionRange = selection_range,
            });
        }
    }
}

static void Parse(std::vector<SkSLSymbol>* symbols, const SkSL::StructDefinition& element, std::string_view content) {
    auto range = SkSLRange(element.position());
    range.Join(element.type().position());

    auto name = element.type().name();
    auto selection_range = Find(content, element.type().position(), name);

    symbols->push_back({
        .name = std::string(name),
        .detail = "struct",
        .kind = "struct",
        .range = range,
        .selectionRange = selection_range,
        .children = ToFieldSymbols(element.type(), content),
    });
}

GetSymbolResult GetSymbol(Modules* modules, const GetSymbolParams& params) {
    GetSymbolResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    for (const auto& element : iter->second.program->fOwnedElements) {
        switch (element->kind()) {
        case SkSL::ProgramElementKind::kExtension: {
            auto& extension = element->as<SkSL::Extension>();
            std::cerr << "TODO: GetSymbol kExtension " << std::endl;
            break;
        }
        case SkSL::ProgramElementKind::kFunction: {
            auto& function = element->as<SkSL::FunctionDefinition>();
            Parse(&result.symbols, function, iter->second.content);
            break;
        }
        case SkSL::ProgramElementKind::kFunctionPrototype: {
            auto& prototype = element->as<SkSL::FunctionPrototype>();
            Parse(&result.symbols, prototype, iter->second.content);
            break;
        }
        case SkSL::ProgramElementKind::kGlobalVar: {
            auto& var = element->as<SkSL::GlobalVarDeclaration>();
            Parse(&result.symbols, var, iter->second.content);
            break;
        }
        case SkSL::ProgramElementKind::kInterfaceBlock: {
            auto& interface = element->as<SkSL::InterfaceBlock>();
            Parse(&result.symbols, interface, iter->second.content);
            break;
        }
        case SkSL::ProgramElementKind::kModifiers: {
            auto& modifiers = element->as<SkSL::ModifiersDeclaration>();
            std::cerr << "TODO: GetSymbol kModifiers " << std::endl;
            break;
        }
        case SkSL::ProgramElementKind::kStructDefinition: {
            auto& struct_d = element->as<SkSL::StructDefinition>();
            Parse(&result.symbols, struct_d, iter->second.content);
            break;
        }
        }
    }

    return result;
}
