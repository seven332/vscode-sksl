#pragma once

#include <src/sksl/ir/SkSLProgram.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "token.h"

class Module {
 public:
    Module() = default;

    Module(std::string content) : content_backend_(std::move(content)) {
        content_ = content_backend_;
    }

    Module(std::string_view content, std::unique_ptr<SkSL::Program> program, std::vector<Token> tokens) :
        content_(content),
        program_(std::move(program)),
        tokens_(std::move(tokens)) {}

    [[nodiscard]] bool HasProgram() const {
        return program_ != nullptr;
    }

    [[nodiscard]] std::string_view GetContent() const {
        return content_;
    }

    [[nodiscard]] const std::unique_ptr<SkSL::Program>& GetProgram() const {
        return program_;
    }

    [[nodiscard]] const std::vector<Token>& GetTokens() const {
        return tokens_;
    }

 private:
    std::string_view content_;
    std::unique_ptr<SkSL::Program> program_;
    std::vector<Token> tokens_;
    std::string content_backend_;
};

using Modules = std::unordered_map<std::string, Module>;
