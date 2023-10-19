#include "get_token.h"

#include <src/sksl/SkSLPosition.h>
#include <src/sksl/ir/SkSLBinaryExpression.h>
#include <src/sksl/ir/SkSLBlock.h>
#include <src/sksl/ir/SkSLChildCall.h>
#include <src/sksl/ir/SkSLConstructor.h>
#include <src/sksl/ir/SkSLDoStatement.h>
#include <src/sksl/ir/SkSLExpression.h>
#include <src/sksl/ir/SkSLExpressionStatement.h>
#include <src/sksl/ir/SkSLExtension.h>
#include <src/sksl/ir/SkSLFieldAccess.h>
#include <src/sksl/ir/SkSLFieldSymbol.h>
#include <src/sksl/ir/SkSLForStatement.h>
#include <src/sksl/ir/SkSLFunctionCall.h>
#include <src/sksl/ir/SkSLFunctionDeclaration.h>
#include <src/sksl/ir/SkSLFunctionDefinition.h>
#include <src/sksl/ir/SkSLFunctionPrototype.h>
#include <src/sksl/ir/SkSLIfStatement.h>
#include <src/sksl/ir/SkSLIndexExpression.h>
#include <src/sksl/ir/SkSLInterfaceBlock.h>
#include <src/sksl/ir/SkSLPostfixExpression.h>
#include <src/sksl/ir/SkSLPrefixExpression.h>
#include <src/sksl/ir/SkSLReturnStatement.h>
#include <src/sksl/ir/SkSLSetting.h>
#include <src/sksl/ir/SkSLStructDefinition.h>
#include <src/sksl/ir/SkSLSwitchCase.h>
#include <src/sksl/ir/SkSLSwitchStatement.h>
#include <src/sksl/ir/SkSLSwizzle.h>
#include <src/sksl/ir/SkSLSymbol.h>
#include <src/sksl/ir/SkSLTernaryExpression.h>
#include <src/sksl/ir/SkSLType.h>
#include <src/sksl/ir/SkSLTypeReference.h>
#include <src/sksl/ir/SkSLVarDeclarations.h>
#include <src/sksl/ir/SkSLVariable.h>
#include <src/sksl/ir/SkSLVariableReference.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string_view>
#include <unordered_set>

#include "data.h"
#include "lexer.h"

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

struct Context {
    std::string_view content;
    std::vector<SkSLToken>* tokens;
    std::unordered_set<const SkSL::ProgramElement*> elements;
    std::unordered_set<const SkSL::Statement*> statements;
    std::unordered_set<const SkSL::Expression*> expressions;
    std::unordered_set<const SkSL::Symbol*> symbols;
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

static SkSLRange FindIdentifier(std::string_view content, std::string_view text, SkSL::Position position) {
    auto fragment = content.substr(position.startOffset(), position.endOffset() - position.startOffset());
    auto range = FindIdentifier(fragment, text);
    if (range.IsValid()) {
        range.Offset(position.startOffset());
    }
    return range;
}

static void Push(Context* context, const SkSLRange& range, TokenType token_type, std::uint32_t token_modifiers) {
    context->tokens->push_back(SkSLToken {
        .range = range,
        .tokenType = static_cast<std::uint32_t>(token_type),
        .tokenModifiers = token_modifiers,
    });
}

static void Push(std::string_view text, Context* context, TokenType token_type, std::uint32_t token_modifiers) {
    auto range = Find(context->content, text);
    if (range.IsValid()) {
        Push(context, range, token_type, token_modifiers);
    }
}

static void PushReference(const SkSL::Type* type, SkSL::Position position, Context* context) {
    if (!type) {
        return;
    }
    auto range = FindIdentifier(context->content, type->name(), position);
    if (type->isStruct()) {
        Push(context, range, TokenType::kStruct, type->isInBuiltinTypes() ? TokenModifier::kDefaultLibrary : 0);
    }
}

static void PushReference(const SkSL::FunctionDeclaration* func, SkSL::Position position, Context* context) {
    if (!func) {
        return;
    }
    auto range = FindIdentifier(context->content, func->name(), position);
    Push(context, range, TokenType::kFunction, func->isBuiltin() ? TokenModifier::kDefaultLibrary : 0);
}

static void PushReference(const SkSL::Variable* var, SkSL::Position position, Context* context) {
    if (!var) {
        return;
    }
    auto range = FindIdentifier(context->content, var->name(), position);
    auto type = var->storage() == SkSL::VariableStorage::kParameter ? TokenType::kParameter : TokenType::kVariable;
    std::uint32_t modifiers = 0;
    if (var->modifierFlags().isConst() || var->modifierFlags().isReadOnly()) {
        modifiers |= TokenModifier::kReadonly;
    }
    if (var->isBuiltin()) {
        modifiers |= TokenModifier::kDefaultLibrary;
    }
    Push(context, range, type, modifiers);
}

static void PushReference(const SkSL::Field* field, SkSL::Position position, Context* context) {
    if (!field) {
        return;
    }
    auto range = FindIdentifier(context->content, field->fName, position);
    Push(context, range, TokenType::kProperty, 0);
}

// NOLINTNEXTLINE
static void Parse(const SkSL::Symbol* symbol, Context* context) {
    if (!symbol || !context->symbols.insert(symbol).second) {
        return;
    }

    switch (symbol->kind()) {
    case SkSL::SymbolKind::kExternal:
        // None
        break;
    case SkSL::SymbolKind::kField: {
        const auto& fs = symbol->as<SkSL::FieldSymbol>();
        Push(fs.name(), context, TokenType::kProperty, 0);
        PushReference(&fs.type(), fs.position(), context);
        break;
    }
    case SkSL::SymbolKind::kFunctionDeclaration: {
        const auto& fd = symbol->as<SkSL::FunctionDeclaration>();
        Push(fd.name(), context, TokenType::kFunction, 0);
        for (const auto* parameter : fd.parameters()) {
            Parse(parameter, context);
        }
        break;
    }
    case SkSL::SymbolKind::kType: {
        const auto& t = symbol->as<SkSL::Type>();
        if (t.isStruct()) {
            Push(t.name(), context, TokenType::kStruct, 0);
            for (const auto& field : t.fields()) {
                Push(field.fName, context, TokenType::kProperty, 0);
                PushReference(field.fType, field.fPosition, context);
            }
        }
        break;
    }
    case SkSL::SymbolKind::kVariable: {
        const auto& v = symbol->as<SkSL::Variable>();
        PushReference(&v.type(), v.position(), context);

        auto type = v.storage() == SkSL::VariableStorage::kParameter ? TokenType::kParameter : TokenType::kVariable;
        std::uint32_t modifiers = 0;
        if (v.modifierFlags().isConst() || v.modifierFlags().isReadOnly()) {
            modifiers |= TokenModifier::kReadonly;
        }
        if (v.isBuiltin()) {
            modifiers |= TokenModifier::kDefaultLibrary;
        }
        Push(v.name(), context, type, modifiers);
        break;
    }
    }
}

// NOLINTNEXTLINE
static void Parse(const SkSL::Expression* expression, Context* context) {
    if (!expression || !context->expressions.insert(expression).second) {
        return;
    }

    switch (expression->kind()) {
    case SkSL::ExpressionKind::kBinary: {
        const auto& b = expression->as<SkSL::BinaryExpression>();
        Parse(b.left().get(), context);
        Parse(b.right().get(), context);
        break;
    }
    case SkSL::ExpressionKind::kChildCall: {
        const auto& cc = expression->as<SkSL::ChildCall>();
        PushReference(&cc.child(), cc.position(), context);

        auto range = FindIdentifier(context->content, "eval", cc.position());
        if (range.IsValid()) {
            Push(context, range, TokenType::kFunction, TokenModifier::kDefaultLibrary);
        }

        for (const auto& child : cc.arguments()) {
            Parse(child.get(), context);
        }
        break;
    }
    case SkSL::ExpressionKind::kConstructorArray:
    case SkSL::ExpressionKind::kConstructorArrayCast:
    case SkSL::ExpressionKind::kConstructorCompound:
    case SkSL::ExpressionKind::kConstructorCompoundCast:
    case SkSL::ExpressionKind::kConstructorDiagonalMatrix:
    case SkSL::ExpressionKind::kConstructorMatrixResize:
    case SkSL::ExpressionKind::kConstructorScalarCast:
    case SkSL::ExpressionKind::kConstructorSplat:
    case SkSL::ExpressionKind::kConstructorStruct: {
        const auto& ac = expression->as<SkSL::AnyConstructor>();
        PushReference(&ac.type(), ac.position(), context);
        for (const auto& child : ac.argumentSpan()) {
            Parse(child.get(), context);
        }
        break;
    }
    case SkSL::ExpressionKind::kEmpty:
        // None
        break;
    case SkSL::ExpressionKind::kFieldAccess: {
        const auto& fa = expression->as<SkSL::FieldAccess>();

        Parse(fa.base().get(), context);

        auto start = fa.base()->position().endOffset();
        auto end = fa.position().endOffset();
        if (start < end) {
            auto position = SkSL::Position::Range(start, end);
            PushReference(&fa.base()->type().fields()[fa.fieldIndex()], position, context);
        }
        break;
    }
    case SkSL::ExpressionKind::kFunctionReference:
        // None
        break;
    case SkSL::ExpressionKind::kFunctionCall: {
        const auto& fc = expression->as<SkSL::FunctionCall>();
        PushReference(&fc.function(), fc.position(), context);
        for (const auto& child : fc.arguments()) {
            Parse(child.get(), context);
        }
        break;
    }
    case SkSL::ExpressionKind::kIndex: {
        const auto& ie = expression->as<SkSL::IndexExpression>();
        Parse(ie.base().get(), context);
        Parse(ie.index().get(), context);
        break;
    }
    case SkSL::ExpressionKind::kLiteral:
    case SkSL::ExpressionKind::kMethodReference:
    case SkSL::ExpressionKind::kPoison:
        // None
        break;
    case SkSL::ExpressionKind::kPostfix: {
        const auto& pe = expression->as<SkSL::PostfixExpression>();
        Parse(pe.operand().get(), context);
        break;
    }
    case SkSL::ExpressionKind::kPrefix: {
        const auto& pe = expression->as<SkSL::PrefixExpression>();
        Parse(pe.operand().get(), context);
        break;
    }
    case SkSL::ExpressionKind::kSetting: {
        const auto& s = expression->as<SkSL::Setting>();

        auto range = FindIdentifier(context->content, "sk_Caps", s.position());
        if (range.IsValid()) {
            Push(context, range, TokenType::kVariable, TokenModifier::kDefaultLibrary);
        }

        range = FindIdentifier(context->content, s.name(), s.position());
        if (range.IsValid()) {
            Push(context, range, TokenType::kProperty, 0);
        }
        break;
    }
    case SkSL::ExpressionKind::kSwizzle: {
        const auto& s = expression->as<SkSL::Swizzle>();

        Parse(s.base().get(), context);

        auto start = s.base()->position().endOffset();
        auto end = s.position().endOffset();
        if (start < end) {
            auto position = SkSL::Position::Range(start, end);
            auto components = SkSL::Swizzle::MaskString(s.components());
            auto range = FindIdentifier(context->content, components, position);
            if (range.IsValid()) {
                Push(context, range, TokenType::kProperty, 0);
            }
        }
        break;
    }
    case SkSL::ExpressionKind::kTernary: {
        const auto& te = expression->as<SkSL::TernaryExpression>();
        Parse(te.test().get(), context);
        Parse(te.ifTrue().get(), context);
        Parse(te.ifFalse().get(), context);
        break;
    }
    case SkSL::ExpressionKind::kTypeReference: {
        const auto& tr = expression->as<SkSL::TypeReference>();
        PushReference(&tr.type(), tr.position(), context);
        break;
    }
    case SkSL::ExpressionKind::kVariableReference: {
        const auto& vr = expression->as<SkSL::VariableReference>();
        PushReference(vr.variable(), vr.position(), context);
        break;
    }
    }
}

// NOLINTNEXTLINE
static void Parse(const SkSL::Statement* statement, Context* context) {
    if (!statement || !context->statements.insert(statement).second) {
        return;
    }

    switch (statement->kind()) {
    case SkSL::StatementKind::kBlock: {
        const auto& b = statement->as<SkSL::Block>();
        for (const auto& child : b.children()) {
            Parse(child.get(), context);
        }
        break;
    }
    case SkSL::StatementKind::kBreak:
    case SkSL::StatementKind::kContinue:
    case SkSL::StatementKind::kDiscard:
        // None
        break;
    case SkSL::StatementKind::kDo: {
        const auto& ds = statement->as<SkSL::DoStatement>();
        Parse(ds.statement().get(), context);
        Parse(ds.test().get(), context);
        break;
    }
    case SkSL::StatementKind::kExpression: {
        const auto& es = statement->as<SkSL::ExpressionStatement>();
        Parse(es.expression().get(), context);
        break;
    }
    case SkSL::StatementKind::kFor: {
        const auto& fs = statement->as<SkSL::ForStatement>();
        Parse(fs.initializer().get(), context);
        Parse(fs.test().get(), context);
        Parse(fs.next().get(), context);
        Parse(fs.statement().get(), context);
        break;
    }
    case SkSL::StatementKind::kIf: {
        const auto& is = statement->as<SkSL::IfStatement>();
        Parse(is.test().get(), context);
        Parse(is.ifTrue().get(), context);
        Parse(is.ifFalse().get(), context);
        break;
    }
    case SkSL::StatementKind::kNop:
        // None
        break;
    case SkSL::StatementKind::kReturn: {
        const auto& fs = statement->as<SkSL::ReturnStatement>();
        Parse(fs.expression().get(), context);
        break;
    }
    case SkSL::StatementKind::kSwitch: {
        const auto& ss = statement->as<SkSL::SwitchStatement>();
        Parse(ss.value().get(), context);
        for (const auto& child : ss.cases()) {
            Parse(child.get(), context);
        }
        break;
    }
    case SkSL::StatementKind::kSwitchCase: {
        const auto& sc = statement->as<SkSL::SwitchCase>();
        Parse(sc.statement().get(), context);
        break;
    }
    case SkSL::StatementKind::kVarDeclaration: {
        const auto& vd = statement->as<SkSL::VarDeclaration>();
        Parse(vd.var(), context);
        Parse(vd.value().get(), context);
        break;
    }
    }
}

static void Parse(const SkSL::ProgramElement* element, Context* context) {
    if (!element || !context->elements.insert(element).second) {
        return;
    }

    switch (element->kind()) {
    case SkSL::ProgramElementKind::kExtension: {
        const auto& e = element->as<SkSL::Extension>();
        Push(e.name(), context, TokenType::kDecorator, 0);
        break;
    }
    case SkSL::ProgramElementKind::kFunction: {
        const auto& fd = element->as<SkSL::FunctionDefinition>();
        Parse(&fd.declaration(), context);
        Parse(fd.body().get(), context);
        break;
    }
    case SkSL::ProgramElementKind::kFunctionPrototype: {
        const auto& fp = element->as<SkSL::FunctionPrototype>();
        Parse(&fp.declaration(), context);
        break;
    }
    case SkSL::ProgramElementKind::kGlobalVar: {
        const auto& gvd = element->as<SkSL::GlobalVarDeclaration>();
        Parse(gvd.declaration().get(), context);
        break;
    }
    case SkSL::ProgramElementKind::kInterfaceBlock: {
        const auto& ib = element->as<SkSL::InterfaceBlock>();
        Parse(ib.var(), context);
        break;
    }
    case SkSL::ProgramElementKind::kModifiers:
        // None
        break;
    case SkSL::ProgramElementKind::kStructDefinition:
        const auto& sd = element->as<SkSL::StructDefinition>();
        Parse(&sd.type(), context);
        break;
    }
}

GetTokenResult GetToken(Modules* modules, const GetTokenParams& params) {
    GetTokenResult result;

    auto iter = modules->find(params.file);
    if (iter == modules->end()) {
        return result;
    }

    Context context {
        .content = iter->second.content,
        .tokens = &result.tokens,
    };
    for (const auto& element : iter->second.program->fOwnedElements) {
        Parse(element.get(), &context);
    }

    std::sort(result.tokens.begin(), result.tokens.end(), [](const SkSLToken& a, const SkSLToken& b) {
        return a.range.start < b.range.start;
    });

    return result;
}
