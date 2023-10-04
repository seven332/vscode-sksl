#include "action/get_symbol.h"

#include <src/sksl/ir/SkSLExtension.h>
#include <src/sksl/ir/SkSLFunctionDefinition.h>
#include <src/sksl/ir/SkSLFunctionPrototype.h>
#include <src/sksl/ir/SkSLInterfaceBlock.h>
#include <src/sksl/ir/SkSLModifiersDeclaration.h>
#include <src/sksl/ir/SkSLStructDefinition.h>
#include <src/sksl/ir/SkSLVarDeclarations.h>

#include <iostream>

GetSymbolResult GetSymbol(Modules* modules, const GetSymbolParams& params) {
    GetSymbolResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

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

    return result;
}
