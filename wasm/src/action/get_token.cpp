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
};

enum TokenModifier : std::uint32_t {
    kReadonly = 1,
    kDefaultLibrary = 2,
};

template<class>
inline constexpr bool kAlwaysFalseV = false;

GetTokenResult GetToken(Modules* modules, const GetTokenParams& params) {
    GetTokenResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    for (const auto& token : iter->second.tokens) {
        TokenType token_type = TokenType::kClass;
        std::uint32_t token_modifiers = 0;
        bool should_continue = false;
        std::visit(
            [&token, &token_type, &token_modifiers, &should_continue](auto&& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, const SkSL::Extension*>) {
                    token_type = TokenType::kDecorator;
                    token_modifiers |= TokenModifier::kDefaultLibrary;
                } else if constexpr (std::is_same_v<T, const SkSL::FunctionDeclaration*>) {
                    token_type = TokenType::kFunction;
                    if (value->isBuiltin()) {
                        token_modifiers |= TokenModifier::kDefaultLibrary;
                    }
                } else if constexpr (std::is_same_v<T, const SkSL::Variable*>) {
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
                } else if constexpr (std::is_same_v<T, const SkSL::Type*>) {
                    if (value->isStruct()) {
                        token_type = TokenType::kStruct;
                        if (value->isInBuiltinTypes()) {
                            token_modifiers |= TokenModifier::kDefaultLibrary;
                        }
                    } else {
                        // TODO:
                        should_continue = true;
                    }
                } else if constexpr (std::is_same_v<T, const SkSL::Field*>) {
                    token_type = TokenType::kProperty;
                } else if constexpr (std::is_same_v<T, const SkSL::FieldSymbol*>) {
                    token_type = TokenType::kProperty;
                } else if constexpr (std::is_same_v<T, const SkSL::Literal*>) {
                    // TODO:
                    should_continue = true;
                } else if constexpr (std::is_same_v<T, const SkSL::Setting*>) {
                    token_type = TokenType::kProperty;
                } else if constexpr (std::is_same_v<T, const SkSL::Swizzle*>) {
                    token_type = TokenType::kProperty;
                } else if constexpr (std::is_same_v<T, const SkSL::ChildCall*>) {
                    token_type = TokenType::kFunction;
                    token_modifiers |= TokenModifier::kDefaultLibrary;
                } else {
                    static_assert(kAlwaysFalseV<T>, "non-exhaustive visitor!");
                }
            },
            token.value
        );
        if (should_continue) {
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
