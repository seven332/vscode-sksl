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
            [&token_type, &skip](const SkSL::Literal* value) {
                if (value->isFloatLiteral() || value->isIntLiteral()) {
                    token_type = TokenType::kNumber;
                } else {
                    // It's a bool
                    skip = true;
                }
            },
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

static void ToUTF16Range(std::vector<SkSLToken>* result, const UTF16Index& utf16_index) {
    for (auto& token : *result) {
        token.range.start = utf16_index.ToUTF16(token.range.start);
        token.range.end = utf16_index.ToUTF16(token.range.end);
    }
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

    // Convert range to utf-16
    ToUTF16Range(&result.tokens, iter->second.utf16_index);

    return result;
}

GetTokenRangeResult GetTokenRange(Modules* modules, const GetTokenRangeParams& params) {  // NOLINT
    GetTokenRangeResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    // Convert range to utf-8
    auto range = params.range;
    range.start = iter->second.utf16_index.ToUTF8(range.start);
    range.end = iter->second.utf16_index.ToUTF8(range.end);

    // Find the first token that (range.start < token.range.end) is true
    auto i = std::upper_bound(
        iter->second.tokens.begin(),
        iter->second.tokens.end(),
        range,
        [](const SkSLRange& range, const Token& token) { return range.start < token.range.end; }
    );

    for (; i != iter->second.tokens.end() && i->range.Intersects(range); ++i) {
        Convert(&result.tokens, *i);
    }

    // Convert range to utf-16
    ToUTF16Range(&result.tokens, iter->second.utf16_index);

    return result;
}
