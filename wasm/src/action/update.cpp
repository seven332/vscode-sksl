#include "action/update.h"

#include <src/sksl/SkSLCompiler.h>
#include <src/sksl/SkSLErrorReporter.h>
#include <src/sksl/SkSLModuleLoader.h>
#include <src/sksl/SkSLParser.h>
#include <src/sksl/SkSLPosition.h>
#include <src/sksl/SkSLProgramKind.h>
#include <src/sksl/SkSLProgramSettings.h>
#include <src/sksl/SkSLUtil.h>
#include <src/sksl/ir/SkSLBinaryExpression.h>
#include <src/sksl/ir/SkSLConstructor.h>
#include <src/sksl/ir/SkSLDoStatement.h>
#include <src/sksl/ir/SkSLExpression.h>
#include <src/sksl/ir/SkSLExpressionStatement.h>
#include <src/sksl/ir/SkSLExtension.h>
#include <src/sksl/ir/SkSLFieldAccess.h>
#include <src/sksl/ir/SkSLForStatement.h>
#include <src/sksl/ir/SkSLFunctionCall.h>
#include <src/sksl/ir/SkSLFunctionDefinition.h>
#include <src/sksl/ir/SkSLFunctionPrototype.h>
#include <src/sksl/ir/SkSLIfStatement.h>
#include <src/sksl/ir/SkSLIndexExpression.h>
#include <src/sksl/ir/SkSLInterfaceBlock.h>
#include <src/sksl/ir/SkSLLiteral.h>
#include <src/sksl/ir/SkSLPostfixExpression.h>
#include <src/sksl/ir/SkSLPrefixExpression.h>
#include <src/sksl/ir/SkSLProgramElement.h>
#include <src/sksl/ir/SkSLReturnStatement.h>
#include <src/sksl/ir/SkSLStatement.h>
#include <src/sksl/ir/SkSLStructDefinition.h>
#include <src/sksl/ir/SkSLSwitchCase.h>
#include <src/sksl/ir/SkSLSwitchStatement.h>
#include <src/sksl/ir/SkSLSymbol.h>
#include <src/sksl/ir/SkSLTernaryExpression.h>
#include <src/sksl/ir/SkSLTypeReference.h>
#include <src/sksl/ir/SkSLVarDeclarations.h>
#include <src/sksl/ir/SkSLVariableReference.h>

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "data.h"
#include "hash.h"
#include "lexer.h"
#include "token.h"

#pragma mark - Compile

class SkSLErrorReporter : public SkSL::ErrorReporter {
 public:
    [[nodiscard]] const std::vector<SkSLError>& GetErrors() const {
        return errors_;
    }

    void FetchErrors(std::vector<SkSLError>* errors) {
        *errors = std::move(errors_);
    }

 protected:
    void handleError(std::string_view msg, SkSL::Position position) override {
        errors_.push_back({
            .message = std::string(msg),
            .range = position,
        });
    }

 private:
    std::vector<SkSLError> errors_;
};

static SkSL::ProgramKind ToProgramKind(const std::string& kind) {
    switch (Hash(kind.data())) {
    case Hash("shader"):
        return SkSL::ProgramKind::kRuntimeShader;
    case Hash("colorfilter"):
        return SkSL::ProgramKind::kRuntimeColorFilter;
    case Hash("blender"):
        return SkSL::ProgramKind::kRuntimeBlender;
    case Hash("meshfrag"):
        return SkSL::ProgramKind::kMeshFragment;
    case Hash("meshvert"):
        return SkSL::ProgramKind::kMeshVertex;
    default:
        std::cerr << "Abort: invalid program kind: " << kind << '\n';
        std::abort();
    }
}

static std::unique_ptr<SkSL::Program> CompileProgram(
    SkSL::Compiler* compiler,
    SkSL::ProgramKind kind,
    const char* module_name,
    std::string module_source,
    SkSLErrorReporter* error_reporter
) {
    SkSL::ProgramSettings settings;
    settings.fOptimize = false;
    settings.fRemoveDeadFunctions = false;
    settings.fRemoveDeadVariables = false;
    if (SkSL::ProgramConfig::IsRuntimeEffect(kind)) {
        settings.fAllowNarrowingConversions = true;
    }
    SkSL::Parser parser {compiler, settings, kind, std::move(module_source)};
    auto program = parser.program();
    if (error_reporter && (!error_reporter->GetErrors().empty() || !program)) {
        std::cerr << "Abort: failed to complied " << module_name << '\n';
        for (const auto& error : error_reporter->GetErrors()) {
            std::cerr << error.message << '\n';
        }
        std::abort();
    }
    return program;
}

#pragma mark - Parse

struct Context {
    std::vector<Token>* tokens;
    std::string_view content;
};

static SkSLRange Range(std::string_view content, std::string_view text) {
    if (text.data() >= content.data() && text.data() + text.size() <= content.data() + content.size()) {
        std::uint32_t start = text.data() - content.data();
        std::uint32_t end = start + text.size();
        return SkSLRange(start, end);
    } else {
        return {};
    }
}

static void PushOrigin(Context* context, std::string_view text, Token::Value value) {
    auto range = Range(context->content, text);
    if (!range.IsValid()) {
        return;
    }
    context->tokens->push_back(Token {.value = value, .range = range, .is_reference = false});
}

static void PushOrigin(Context* context, const SkSL::Position& position, Token::Value value) {
    context->tokens->push_back(Token {.value = value, .range = position, .is_reference = false});
}

static SkSLRange Find(std::string_view content, std::string_view text, SkSL::Position position) {
    auto fragment = content.substr(position.startOffset(), position.endOffset() - position.startOffset());
    auto range = FindIdentifier(fragment, text);
    if (range.IsValid()) {
        range.Offset(position.startOffset());
    }
    return range;
}

static void PushReference(Context* context, std::string_view text, SkSL::Position position, Token::Value value) {
    auto range = Find(context->content, text, position);
    if (!range.IsValid()) {
        return;
    }
    context->tokens->push_back(Token {.value = value, .range = range, .is_reference = true});
}

static void Parse(Context* context, const SkSL::Symbol* symbol) {
    if (!symbol) {
        return;
    }

    switch (symbol->kind()) {
    case SkSL::SymbolKind::kExternal:
        // None
        break;
    case SkSL::SymbolKind::kField: {
        const auto& fs = symbol->as<SkSL::FieldSymbol>();
        PushReference(context, fs.type().name(), fs.position(), &fs.type());
        PushOrigin(context, fs.name(), &fs);
        break;
    }
    case SkSL::SymbolKind::kFunctionDeclaration: {
        const auto& fd = symbol->as<SkSL::FunctionDeclaration>();
        PushOrigin(context, fd.name(), &fd);
        for (const auto* parameter : fd.parameters()) {
            PushOrigin(context, parameter->name(), parameter);
        }
        break;
    }
    case SkSL::SymbolKind::kType: {
        const auto& t = symbol->as<SkSL::Type>();
        PushOrigin(context, t.name(), &t);
        if (t.isStruct()) {
            for (const auto& field : t.fields()) {
                PushReference(context, field.fType->name(), field.fPosition, field.fType);
                PushOrigin(context, field.fName, &field);
            }
        }
        break;
    }
    case SkSL::SymbolKind::kVariable: {
        const auto& v = symbol->as<SkSL::Variable>();
        PushReference(context, v.type().name(), v.position(), &v.type());
        PushOrigin(context, v.name(), &v);
        break;
    }
    }
}

static void Parse(Context* context, const SkSL::Expression* expression) {  // NOLINT
    if (!expression) {
        return;
    }

    switch (expression->kind()) {
    case SkSL::ExpressionKind::kBinary: {
        const auto& b = expression->as<SkSL::BinaryExpression>();
        Parse(context, b.left().get());
        Parse(context, b.right().get());
        break;
    }
    case SkSL::ExpressionKind::kChildCall: {
        const auto& cc = expression->as<SkSL::ChildCall>();
        PushReference(context, cc.child().name(), cc.position(), &cc.child());
        PushReference(context, "eval", cc.position(), &cc);
        for (const auto& argument : cc.arguments()) {
            Parse(context, argument.get());
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
        const auto& ac = *reinterpret_cast<const SkSL::AnyConstructor*>(expression);
        PushReference(context, ac.type().name(), ac.position(), &ac.type());
        for (const auto& argument : ac.argumentSpan()) {
            Parse(context, argument.get());
        }
        break;
    }
    case SkSL::ExpressionKind::kEmpty:
        // None
        break;
    case SkSL::ExpressionKind::kFieldAccess: {
        const auto& fa = expression->as<SkSL::FieldAccess>();

        Parse(context, fa.base().get());

        auto start = fa.base()->position().endOffset();
        auto end = fa.position().endOffset();
        if (start < end) {
            auto position = SkSL::Position::Range(start, end);
            const auto& field = fa.base()->type().fields()[fa.fieldIndex()];
            PushReference(context, field.fName, position, &field);
        }
        break;
    }
    case SkSL::ExpressionKind::kFunctionReference:
        // None
        break;
    case SkSL::ExpressionKind::kFunctionCall: {
        const auto& fc = expression->as<SkSL::FunctionCall>();
        PushReference(context, fc.function().name(), fc.position(), &fc.function());
        for (const auto& argument : fc.arguments()) {
            Parse(context, argument.get());
        }
        break;
    }
    case SkSL::ExpressionKind::kIndex: {
        const auto& ie = expression->as<SkSL::IndexExpression>();
        Parse(context, ie.base().get());
        Parse(context, ie.index().get());
        break;
    }
    case SkSL::ExpressionKind::kLiteral: {
        const auto& l = expression->as<SkSL::Literal>();
        PushOrigin(context, l.position(), &l);
        break;
    }
    case SkSL::ExpressionKind::kMethodReference:
    case SkSL::ExpressionKind::kPoison:
        // None
        break;
    case SkSL::ExpressionKind::kPostfix: {
        const auto& pe = expression->as<SkSL::PostfixExpression>();
        Parse(context, pe.operand().get());
        break;
    }
    case SkSL::ExpressionKind::kPrefix: {
        const auto& pe = expression->as<SkSL::PrefixExpression>();
        Parse(context, pe.operand().get());
        break;
    }
    case SkSL::ExpressionKind::kSetting: {
        const auto& s = expression->as<SkSL::Setting>();
        // TODO: sk_Caps
        PushReference(context, s.name(), s.position(), &s);
        break;
    }
    case SkSL::ExpressionKind::kSwizzle: {
        const auto& s = expression->as<SkSL::Swizzle>();

        Parse(context, s.base().get());

        auto start = s.base()->position().endOffset();
        auto end = s.position().endOffset();
        if (start < end) {
            auto position = SkSL::Position::Range(start, end);
            auto components = SkSL::Swizzle::MaskString(s.components());
            PushReference(context, components, position, &s);
        }
        break;
    }
    case SkSL::ExpressionKind::kTernary: {
        const auto& te = expression->as<SkSL::TernaryExpression>();
        Parse(context, te.test().get());
        Parse(context, te.ifTrue().get());
        Parse(context, te.ifFalse().get());
        break;
    }
    case SkSL::ExpressionKind::kTypeReference: {
        const auto& tr = expression->as<SkSL::TypeReference>();
        PushReference(context, tr.type().name(), tr.position(), &tr.type());
        break;
    }
    case SkSL::ExpressionKind::kVariableReference: {
        const auto& vr = expression->as<SkSL::VariableReference>();
        PushReference(context, vr.variable()->name(), vr.position(), vr.variable());
        break;
    }
    }
}

static void Parse(Context* context, const SkSL::Statement* statement) {  // NOLINT
    if (!statement) {
        return;
    }

    switch (statement->kind()) {
    case SkSL::StatementKind::kBlock: {
        const auto& b = statement->as<SkSL::Block>();
        for (const auto& child : b.children()) {
            Parse(context, child.get());
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
        Parse(context, ds.statement().get());
        Parse(context, ds.test().get());
        break;
    }
    case SkSL::StatementKind::kExpression: {
        const auto& es = statement->as<SkSL::ExpressionStatement>();
        Parse(context, es.expression().get());
        break;
    }
    case SkSL::StatementKind::kFor: {
        const auto& fs = statement->as<SkSL::ForStatement>();
        Parse(context, fs.initializer().get());
        Parse(context, fs.test().get());
        Parse(context, fs.next().get());
        Parse(context, fs.statement().get());
        break;
    }
    case SkSL::StatementKind::kIf: {
        const auto& is = statement->as<SkSL::IfStatement>();
        Parse(context, is.test().get());
        Parse(context, is.ifTrue().get());
        Parse(context, is.ifFalse().get());
        break;
    }
    case SkSL::StatementKind::kNop:
        // None
        break;
    case SkSL::StatementKind::kReturn: {
        const auto& fs = statement->as<SkSL::ReturnStatement>();
        Parse(context, fs.expression().get());
        break;
    }
    case SkSL::StatementKind::kSwitch: {
        const auto& ss = statement->as<SkSL::SwitchStatement>();
        Parse(context, ss.value().get());
        for (const auto& child : ss.cases()) {
            Parse(context, child.get());
        }
        break;
    }
    case SkSL::StatementKind::kSwitchCase: {
        const auto& sc = statement->as<SkSL::SwitchCase>();
        Parse(context, sc.statement().get());
        break;
    }
    case SkSL::StatementKind::kVarDeclaration: {
        const auto& vd = statement->as<SkSL::VarDeclaration>();
        Parse(context, vd.var());
        Parse(context, vd.value().get());
        break;
    }
    }
}

static void Parse(Context* context, const SkSL::ProgramElement* element) {
    if (!element) {
        return;
    }

    switch (element->kind()) {
    case SkSL::ProgramElementKind::kExtension: {
        const auto& e = element->as<SkSL::Extension>();
        PushOrigin(context, e.name(), &e);
        break;
    }
    case SkSL::ProgramElementKind::kFunction: {
        const auto& fd = element->as<SkSL::FunctionDefinition>();
        Parse(context, &fd.declaration());
        Parse(context, fd.body().get());
        break;
    }
    case SkSL::ProgramElementKind::kFunctionPrototype: {
        // TODO:
        break;
    }
    case SkSL::ProgramElementKind::kGlobalVar: {
        const auto& gvd = element->as<SkSL::GlobalVarDeclaration>();
        Parse(context, gvd.declaration().get());
        break;
    }
    case SkSL::ProgramElementKind::kInterfaceBlock: {  // NOLINT
        // TODO:
        break;
    }
    case SkSL::ProgramElementKind::kModifiers: {
        // None
        break;
    }
    case SkSL::ProgramElementKind::kStructDefinition: {
        const auto& sd = element->as<SkSL::StructDefinition>();
        Parse(context, &sd.type());
        break;
    }
    }
}

static void Parse(Context* context, const SkSL::Program* program) {
    if (!program) {
        return;
    }

    for (const auto& element : program->fOwnedElements) {
        Parse(context, element.get());
    }
}

UpdateResult Update(Modules* modules, UpdateParams params) {
    if (params.content.size() <= sizeof(std::string)) {
        // Avoid small string optimization
        return {.succeed = false};
    }

    SkSL::Compiler compiler(SkSL::ShaderCapsFactory::Standalone());

    SkSLErrorReporter error_reporter;
    compiler.context().fErrors = &error_reporter;

    auto kind = ToProgramKind(params.kind);
    std::string_view content = params.content;
    auto program = CompileProgram(&compiler, kind, params.file.c_str(), std::move(params.content), nullptr);

    UpdateResult result;
    error_reporter.FetchErrors(&result.errors);
    result.succeed = program != nullptr;

    if (result.succeed) {
        std::vector<Token> tokens;
        Context context {.tokens = &tokens, .content = content};
        Parse(&context, program.get());
        std::sort(tokens.begin(), tokens.end(), [](const Token& a, const Token& b) {
            return a.range.start < b.range.start;
        });
        tokens.erase(
            std::unique(
                tokens.begin(),
                tokens.end(),
                [](const Token& a, const Token& b) { return a.range.start == b.range.start; }
            ),
            tokens.end()
        );

        (*modules)[std::move(params.file)] = {
            .content = content,
            .program = std::move(program),
            .tokens = std::move(tokens),
        };
    }

    return result;
}
