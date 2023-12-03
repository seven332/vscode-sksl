#include "action/definition.h"

#include "overloaded.h"

static void SetRange(DefinitionResult* result, std::string_view content, std::string_view text) {
    if (text.data() >= content.data() && text.data() + text.size() <= content.data() + content.size()) {
        result->found = true;
        result->range.start = text.data() - content.data();
        result->range.end = result->range.start + text.size();
    }
}

DefinitionResult Definition(Modules* modules, const DefinitionParams& params) {
    DefinitionResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end() || !iter->second.HasProgram()) {
        return result;
    }

    // Find the first token that (position < token.range.end) is true
    auto i = std::upper_bound(
        iter->second.GetTokens().begin(),
        iter->second.GetTokens().end(),
        params.position,
        [](std::uint16_t position, const Token& token) { return position < token.range.end; }
    );
    if (i == iter->second.GetTokens().end()) {
        return result;
    }
    const auto& token = *i;
    if (token.range.start > params.position) {
        return result;
    }
    if (!token.is_reference) {
        return result;
    }

    auto content = iter->second.GetContent();
    std::visit(
        Overloaded {
            [](const SkSL::Extension* /*value*/) {},
            [&result, content](const SkSL::FunctionDeclaration* value) { SetRange(&result, content, value->name()); },
            [&result, content](const SkSL::Variable* value) { SetRange(&result, content, value->name()); },
            [&result, content](const SkSL::Type* value) { SetRange(&result, content, value->name()); },
            [&result, content](const SkSL::Field* value) { SetRange(&result, content, value->fName); },
            [&result, content](const SkSL::FieldSymbol* value) { SetRange(&result, content, value->name()); },
            [](Token::Bool /*value*/) {},
            [](Token::Int /*value*/) {},
            [](Token::Float /*value*/) {},
            [](const SkSL::Setting* /*value*/) {},
            [](const SkSL::Swizzle* /*value*/) {},
            [](const SkSL::ChildCall* /*value*/) {},
        },
        token.value
    );

    return result;
}
