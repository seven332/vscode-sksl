#include "get_token.h"

#include <algorithm>
#include <variant>

#include "data.h"
#include "overloaded.h"
#include "token.h"

enum class TokenType : std::uint32_t {
    kClass,
    kInterface,
    kStruct,
    kParameter,
    kVariable,
    kProperty,
    kDecorator,
    kFunction,
    kNumber,
};

enum TokenModifier : std::uint32_t {
    kReadonly = 1,
    kDefaultLibrary = 2,
};

static void Convert(std::vector<SkSLToken>* result, const Token& token) {
    TokenType token_type = TokenType::kClass;
    std::uint32_t token_modifiers = 0;
    bool skip = false;

    std::visit(
        Overloaded {
            [&token_type, &token_modifiers](const SkSL::Extension* /*value*/) {
                token_type = TokenType::kDecorator;
                token_modifiers |= TokenModifier::kDefaultLibrary;
            },
            [&token_type, &token_modifiers](const SkSL::FunctionDeclaration* value) {
                token_type = TokenType::kFunction;
                if (value->isBuiltin()) {
                    token_modifiers |= TokenModifier::kDefaultLibrary;
                }
            },
            [&token_type, &token_modifiers](const SkSL::Variable* value) {
                if (value->storage() == SkSL::VariableStorage::kParameter) {
                    token_type = TokenType::kParameter;
                } else {
                    token_type = TokenType::kVariable;
                }
                if (value->modifierFlags().isConst() || value->modifierFlags().isReadOnly()) {
                    token_modifiers |= TokenModifier::kReadonly;
                }
                if (value->isBuiltin()) {
                    token_modifiers |= TokenModifier::kDefaultLibrary;
                }
            },
            [&token_type, &token_modifiers, &skip](const SkSL::Type* value) {
                if (value->isStruct()) {
                    token_type = TokenType::kStruct;
                    if (value->isInBuiltinTypes()) {
                        token_modifiers |= TokenModifier::kDefaultLibrary;
                    }
                } else {
                    // It's a keyword
                    skip = true;
                }
            },
            [&token_type](const SkSL::Field* /*value*/) { token_type = TokenType::kProperty; },
            [&token_type](const SkSL::FieldSymbol* /*value*/) { token_type = TokenType::kProperty; },
            [&skip](const Token::Bool& /*value*/) { skip = true; },
            [&token_type](const Token::Int& /*value*/) { token_type = TokenType::kNumber; },
            [&token_type](const Token::Float& /*value*/) { token_type = TokenType::kNumber; },
            [&token_type](const SkSL::Setting* /*value*/) { token_type = TokenType::kProperty; },
            [&token_type](const SkSL::Swizzle* /*value*/) { token_type = TokenType::kProperty; },
            [&token_type, &token_modifiers](const SkSL::ChildCall* /*value*/) {
                token_type = TokenType::kFunction;
                token_modifiers |= TokenModifier::kDefaultLibrary;
            },
        },
        token.value
    );
    if (skip) {
        return;
    }
    result->push_back(SkSLToken {
        .range = token.range,
        .token_type = static_cast<std::uint32_t>(token_type),
        .token_modifiers = token_modifiers
    });
}

GetTokenResult GetToken(Modules* modules, const GetTokenParams& params) {  // NOLINT
    GetTokenResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    for (const auto& token : iter->second.tokens) {
        Convert(&result.tokens, token);
    }

    return result;
}

GetTokenRangeResult GetTokenRange(Modules* modules, const GetTokenRangeParams& params) {  // NOLINT
    GetTokenRangeResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    // Find the first token that (range.start < token.range.end) is true
    auto i = std::upper_bound(
        iter->second.tokens.begin(),
        iter->second.tokens.end(),
        params.range,
        [](const SkSLRange& range, const Token& token) { return range.start < token.range.end; }
    );

    for (; i != iter->second.tokens.end() && i->range.Intersects(params.range); ++i) {
        Convert(&result.tokens, *i);
    }

    return result;
}
