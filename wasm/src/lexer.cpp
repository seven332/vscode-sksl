#include "lexer.h"

#include <src/sksl/SkSLLexer.h>

#include "data.h"

using TokenKind = SkSL::Token::Kind;

static SkSLRange Find(std::string_view content, std::string_view name, TokenKind kind) {
    SkSL::Lexer lexer;
    lexer.start(content);
    SkSL::Token token;
    for (;;) {
        token = lexer.next();
        if (token.fKind == TokenKind::TK_END_OF_FILE) {
            break;
        }
        if (token.fKind == kind && content.substr(token.fOffset, token.fLength) == name) {
            return SkSLRange(token.fOffset, token.fOffset + token.fLength);
        }
    }
    return SkSLRange();
}

SkSLRange FindIdentifier(std::string_view content, std::string_view name) {
    if (name.starts_with('$')) {
        return Find(content, name, TokenKind::TK_PRIVATE_IDENTIFIER);
    } else {
        return Find(content, name, TokenKind::TK_IDENTIFIER);
    }
}
