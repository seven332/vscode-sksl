#pragma once

#include <src/sksl/SkSLLexer.h>

#include <string>
#include <string_view>
#include <vector>

class Formatter {
 public:
    std::string Format(std::string_view content);

 private:
    enum class NewLineType {
        kNone,
        kActive,
        kPassive,
    };

    std::string_view content_;
    std::string result_;
    int indent_level_ = 0;
    NewLineType new_line_type_ = NewLineType::kActive;
    std::vector<SkSL::Token> line_tokens_;

    void PipeCommentBeforeNewLine(SkSL::Lexer* lexer);

    [[nodiscard]] bool IsNewLine() const;
    [[nodiscard]] SkSL::Token GetLastValidToken() const;
    [[nodiscard]] SkSL::Token GetSecondLastValidToken() const;
    [[nodiscard]] std::string_view GetTokenText(SkSL::Token token) const;

    void IncreaseIndent();
    void DecreaseIndent();
    void NewLineActively();
    void NewLinePassively();
    void AppendAfterSpace(SkSL::Token token, int count = 1);
    void AppendNoSpace(SkSL::Token token) {
        AppendAfterSpace(token, 0);
    }
    void Append(SkSL::Token token);
    void Append(std::string_view text);
};
