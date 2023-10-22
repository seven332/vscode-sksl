#include "get_token.h"

#include <variant>

#include "data.h"

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

template<class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

GetTokenResult GetToken(Modules* modules, const GetTokenParams& params) {  // NOLINT
    GetTokenResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    for (const auto& token : iter->second.tokens) {
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
            continue;
        }
        result.tokens.push_back(SkSLToken {
            .range = token.range,
            .token_type = static_cast<std::uint32_t>(token_type),
            .token_modifiers = token_modifiers
        });
    }

    return result;
}
