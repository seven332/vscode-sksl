#include "formatter.h"

#include <src/sksl/SkSLLexer.h>

#include <algorithm>
#include <string>
#include <string_view>

using TokenKind = SkSL::Token::Kind;

static bool HasNewLine(std::string_view text) {
    return text.find_first_of('\r') != std::string_view::npos || text.find_first_of('\n') != std::string_view::npos;
}

static int GetNewLines(std::string_view text) {
    auto count = 0;
    bool prepare = false;

    auto flush_prepare = [&count, &prepare]() {
        if (prepare) {
            count += 1;
            prepare = false;
        }
    };

    for (auto c : text) {
        switch (c) {
        case '\n':
            count += 1;
            prepare = false;
            break;
        case '\r':
            flush_prepare();
            prepare = true;
            break;
        default:
            flush_prepare();
            break;
        }
    }
    flush_prepare();
    return count;
}

static bool IsNumber(TokenKind kind) {
    switch (kind) {
    case TokenKind::TK_FLOAT_LITERAL:
    case TokenKind::TK_INT_LITERAL:
    case TokenKind::TK_BAD_OCTAL:
        return true;
    default:
        return false;
    }
}

static bool IsIdentifier(TokenKind kind) {
    switch (kind) {
    case TokenKind::TK_PRIVATE_IDENTIFIER:
    case TokenKind::TK_IDENTIFIER:
        return true;
    default:
        return false;
    }
}

static bool IsExpressionEnd(TokenKind kind) {
    return IsNumber(kind) || IsIdentifier(kind) || kind == TokenKind::TK_RPAREN || kind == TokenKind::TK_RBRACKET;
}

std::string Formatter::Format(std::string_view content) {  // NOLINT
    *this = {};
    content_ = content;

    SkSL::Lexer lexer;
    lexer.start(content_);
    for (;;) {
        SkSL::Token token = lexer.next();
        if (token.fKind == TokenKind::TK_END_OF_FILE) {
            break;
        }

        switch (token.fKind) {
        case TokenKind::TK_NONE:
        case TokenKind::TK_END_OF_FILE:
            break;
        case TokenKind::TK_FLOAT_LITERAL:
        case TokenKind::TK_INT_LITERAL:
        case TokenKind::TK_BAD_OCTAL:
        case TokenKind::TK_PRIVATE_IDENTIFIER:
        case TokenKind::TK_IDENTIFIER: {
            // number and identifier
            auto last = GetLastToken();
            if (last.fKind == TokenKind::TK_PLUSPLUS || last.fKind == TokenKind::TK_MINUSMINUS ||
                last.fKind == TokenKind::TK_BITWISENOT || last.fKind == TokenKind::TK_LOGICALNOT ||
                last.fKind == TokenKind::TK_LPAREN || last.fKind == TokenKind::TK_LBRACKET ||
                last.fKind == TokenKind::TK_DOT) {
                // ++x, --x, ~x, !x, (x, [x, .x
                AppendNoSpace(token);
            } else if (last.fKind == TokenKind::TK_PLUS || last.fKind == TokenKind::TK_MINUS) {
                auto second_last = GetSecondLastToken();
                if (IsExpressionEnd(second_last.fKind)) {
                    AppendAfterSpace(token);
                } else {
                    AppendNoSpace(token);
                }
            } else {
                AppendAfterSpace(token);
            }
            break;
        }
        case TokenKind::TK_TRUE_LITERAL:
        case TokenKind::TK_FALSE_LITERAL:
        case TokenKind::TK_IF:
        case TokenKind::TK_ELSE:
        case TokenKind::TK_FOR:
        case TokenKind::TK_WHILE:
        case TokenKind::TK_DO:
        case TokenKind::TK_SWITCH:
        case TokenKind::TK_CASE:
        case TokenKind::TK_DEFAULT:
        case TokenKind::TK_BREAK:
        case TokenKind::TK_CONTINUE:
        case TokenKind::TK_DISCARD:
        case TokenKind::TK_RETURN:
        case TokenKind::TK_IN:
        case TokenKind::TK_OUT:
        case TokenKind::TK_INOUT:
        case TokenKind::TK_UNIFORM:
        case TokenKind::TK_CONST:
        case TokenKind::TK_FLAT:
        case TokenKind::TK_NOPERSPECTIVE:
        case TokenKind::TK_INLINE:
        case TokenKind::TK_NOINLINE:
        case TokenKind::TK_PURE:
        case TokenKind::TK_READONLY:
        case TokenKind::TK_WRITEONLY:
        case TokenKind::TK_BUFFER:
        case TokenKind::TK_STRUCT:
        case TokenKind::TK_LAYOUT:
        case TokenKind::TK_HIGHP:
        case TokenKind::TK_MEDIUMP:
        case TokenKind::TK_LOWP:
        case TokenKind::TK_ES3:
        case TokenKind::TK_EXPORT:
        case TokenKind::TK_WORKGROUP:
        case TokenKind::TK_PIXELLOCAL:
        case TokenKind::TK_RESERVED:
        case TokenKind::TK_DIRECTIVE: {
            // word
            auto last = GetLastToken();
            if (last.fKind == TokenKind::TK_LPAREN || last.fKind == TokenKind::TK_LBRACKET) {
                AppendNoSpace(token);
            } else {
                AppendAfterSpace(token);
            }
            break;
        }
        case TokenKind::TK_LPAREN: {  // (
            auto last = GetLastToken();
            if (last.fKind == TokenKind::TK_LPAREN || last.fKind == TokenKind::TK_LBRACKET ||
                IsIdentifier(last.fKind)) {
                AppendNoSpace(token);
            } else {
                AppendAfterSpace(token);
            }
            break;
        }
        case TokenKind::TK_RPAREN:  // )
            AppendNoSpace(token);
            break;
        case TokenKind::TK_LBRACE:  // {
            AppendAfterSpace(token);
            PipeBeforeNewLine(&lexer, PipeType::kComment);
            NewLineActively();
            IncreaseIndent();
            break;
        case TokenKind::TK_RBRACE:  // }
            if (!IsNewLine()) {
                NewLineActively();
            }
            DecreaseIndent();
            AppendAfterSpace(token);
            if (PipeBeforeNewLine(&lexer, PipeType::kComment | PipeType::kSemicolon | PipeType::kElse)) {
                NewLineActively();
            }
            break;
        case TokenKind::TK_LBRACKET:  // [
        case TokenKind::TK_RBRACKET:  // ]
        case TokenKind::TK_DOT:       // .
        case TokenKind::TK_COMMA:     // ,
            AppendNoSpace(token);
            break;
        case TokenKind::TK_PLUSPLUS:      // ++
        case TokenKind::TK_MINUSMINUS: {  // --
            auto last = GetLastToken();
            if (IsIdentifier(last.fKind) || IsNumber(last.fKind)) {
                AppendNoSpace(token);
            } else {
                AppendAfterSpace(token);
            }
            break;
        }
        case TokenKind::TK_PLUS:     // +
        case TokenKind::TK_MINUS: {  // -
            auto last = GetLastToken();
            if (last.fKind == TokenKind::TK_LPAREN || last.fKind == TokenKind::TK_LBRACKET) {
                AppendNoSpace(token);
            } else {
                AppendAfterSpace(token);
            }
            break;
        }
        case TokenKind::TK_STAR:          // *
        case TokenKind::TK_SLASH:         // /
        case TokenKind::TK_PERCENT:       // %
        case TokenKind::TK_SHL:           // <<
        case TokenKind::TK_SHR:           // >>
        case TokenKind::TK_BITWISEOR:     // |
        case TokenKind::TK_BITWISEXOR:    // ^
        case TokenKind::TK_BITWISEAND:    // &
        case TokenKind::TK_BITWISENOT:    // ~
        case TokenKind::TK_LOGICALOR:     // ||
        case TokenKind::TK_LOGICALXOR:    // ^^
        case TokenKind::TK_LOGICALAND:    // &&
        case TokenKind::TK_LOGICALNOT:    // !
        case TokenKind::TK_QUESTION:      // ?
        case TokenKind::TK_COLON:         // :
        case TokenKind::TK_EQ:            // =
        case TokenKind::TK_EQEQ:          // ==
        case TokenKind::TK_NEQ:           // !=
        case TokenKind::TK_GT:            // >
        case TokenKind::TK_LT:            // <
        case TokenKind::TK_GTEQ:          // >=
        case TokenKind::TK_LTEQ:          // <=
        case TokenKind::TK_PLUSEQ:        // +=
        case TokenKind::TK_MINUSEQ:       // -=
        case TokenKind::TK_STAREQ:        // *=
        case TokenKind::TK_SLASHEQ:       // /=
        case TokenKind::TK_PERCENTEQ:     // %=
        case TokenKind::TK_SHLEQ:         // <<=
        case TokenKind::TK_SHREQ:         // >>=
        case TokenKind::TK_BITWISEOREQ:   // |=
        case TokenKind::TK_BITWISEXOREQ:  // ^=
        case TokenKind::TK_BITWISEANDEQ:  // &=
            AppendAfterSpace(token);
            break;
        case TokenKind::TK_SEMICOLON: {  // ;
            AppendNoSpace(token);
            PipeBeforeNewLine(&lexer, PipeType::kComment);
            NewLineActively();
            break;
        }
        case TokenKind::TK_WHITESPACE: {
            auto lines = GetNewLines(GetTokenText(token));
            int max_lines = 2;
            auto save = lexer.getCheckpoint();
            if (lexer.next().fKind == TokenKind::TK_END_OF_FILE) {
                max_lines = 1;
            }
            lexer.rewindToCheckpoint(save);
            lines = std::min(lines, max_lines);

            if (new_line_type_ == NewLineType::kActive) {
                lines -= 1;
            }
            for (auto i = 0; i < lines; ++i) {
                NewLinePassively();
            }
            break;
        }
        case TokenKind::TK_LINE_COMMENT:
            AppendAfterSpace(token, 2);
            NewLineActively();
            break;
        case TokenKind::TK_BLOCK_COMMENT:
            AppendAfterSpace(token);
            break;
        case TokenKind::TK_INVALID:
            // failed
            return std::string(content_);
        }
    }

    if (!IsNewLine()) {
        NewLineActively();
    }

    return std::move(result_);
}

bool Formatter::PipeBeforeNewLine(SkSL::Lexer* lexer, std::uint8_t type) {
    for (;;) {
        auto save = lexer->getCheckpoint();
        auto token = lexer->next();
        if (token.fKind == TokenKind::TK_WHITESPACE) {
            if (HasNewLine(GetTokenText(token))) {
                lexer->rewindToCheckpoint(save);
                break;
            }
        } else if ((type & PipeType::kComment) && token.fKind == TokenKind::TK_LINE_COMMENT) {
            AppendAfterSpace(token, 2);
            break;
        } else if ((type & PipeType::kComment) && token.fKind == TokenKind::TK_BLOCK_COMMENT) {
            AppendAfterSpace(token);
            if (HasNewLine(GetTokenText(token))) {
                break;
            }
        } else if ((type & PipeType::kSemicolon) && token.fKind == TokenKind::TK_SEMICOLON) {
            AppendNoSpace(token);
        } else if ((type & PipeType::kElse) && token.fKind == TokenKind::TK_ELSE) {
            AppendAfterSpace(token);
            return false;
        } else {
            lexer->rewindToCheckpoint(save);
            break;
        }
    }
    return true;
}

bool Formatter::IsNewLine() const {
    return new_line_type_ != NewLineType::kNone;
}

SkSL::Token Formatter::GetLastToken() const {
    for (auto iter = line_tokens_.rbegin(); iter != line_tokens_.rend(); ++iter) {  // NOLINT
        switch (iter->fKind) {
        case TokenKind::TK_END_OF_FILE:
        case TokenKind::TK_WHITESPACE:
        case TokenKind::TK_INVALID:
        case TokenKind::TK_NONE:
            break;
        default:
            return *iter;
        }
    }
    return {};
}

SkSL::Token Formatter::GetSecondLastToken() const {
    auto last = false;
    for (auto iter = line_tokens_.rbegin(); iter != line_tokens_.rend(); ++iter) {  // NOLINT
        switch (iter->fKind) {
        case TokenKind::TK_END_OF_FILE:
        case TokenKind::TK_WHITESPACE:
        case TokenKind::TK_LINE_COMMENT:
        case TokenKind::TK_BLOCK_COMMENT:
        case TokenKind::TK_INVALID:
        case TokenKind::TK_NONE:
            break;
        default:
            if (last) {
                return *iter;
            } else {
                last = true;
            }
        }
    }
    return {};
}

std::string_view Formatter::GetTokenText(SkSL::Token token) const {
    return content_.substr(token.fOffset, token.fLength);
}

void Formatter::IncreaseIndent() {
    indent_level_ += 1;
}

void Formatter::DecreaseIndent() {
    indent_level_ = std::max(0, indent_level_ - 1);
}

void Formatter::NewLineActively() {
    Append("\n");
    new_line_type_ = NewLineType::kActive;
    line_tokens_.clear();
}

void Formatter::NewLinePassively() {
    Append("\n");
    new_line_type_ = NewLineType::kPassive;
    line_tokens_.clear();
}

void Formatter::AppendAfterSpace(SkSL::Token token, int count) {
    if (IsNewLine()) {
        for (auto i = 0; i < indent_level_; ++i) {
            Append("    ");
        }
    } else {
        for (auto i = 0; i < count; ++i) {
            Append(" ");
        }
    }
    Append(token);
}

void Formatter::Append(SkSL::Token token) {
    line_tokens_.push_back(token);

    auto text = GetTokenText(token);
    if (token.fKind == TokenKind::TK_LINE_COMMENT) {
        auto not_slash_index = text.find_first_not_of('/');
        if (not_slash_index == std::string_view::npos) {
            Append(text);
            return;
        }
        Append(text.substr(0, not_slash_index));
        auto not_space_index = text.find_first_not_of(' ', not_slash_index);
        if (not_space_index == std::string_view::npos) {
            return;
        }
        Append(" ");
        Append(text.substr(not_space_index));
    } else {
        Append(text);
    }
}

void Formatter::Append(std::string_view text) {
    result_ += text;
    new_line_type_ = NewLineType::kNone;
}
