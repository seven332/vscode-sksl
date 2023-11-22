#pragma once

#include <src/sksl/SkSLLexer.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

class Formatter {
 public:
    std::string Format(std::string_view content);

 private:
    enum class NewLineType : std::uint8_t {
        kNone,
        kActive,
        kPassive,
    };

    enum PipeType : std::uint8_t {
        kComment = 1,
        kSemicolon = 2,
        kElse = 4,
    };

    std::string_view content_;
    std::string result_;
    int indent_level_ = 0;
    NewLineType new_line_type_ = NewLineType::kActive;
    std::vector<SkSL::Token> line_tokens_;

    bool PipeBeforeNewLine(SkSL::Lexer* lexer, std::uint8_t type);

    [[nodiscard]] bool IsNewLine() const;
    [[nodiscard]] bool IsInFor() const;
    [[nodiscard]] SkSL::Token GetLastToken() const;
    [[nodiscard]] SkSL::Token GetLastMaybeEmptyToken() const;
    [[nodiscard]] SkSL::Token GetSecondLastToken() const;
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
