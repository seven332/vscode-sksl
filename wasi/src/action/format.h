#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "module.h"
#include "simple_codec.h"

struct FormatParams {
    std::string file;

    friend std::size_t Read(const std::vector<std::byte>& bytes, std::size_t offset, FormatParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        return read;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FormatParams, file)
};

struct FormatResult {
    std::string newContent;  // NOLINT

    friend void Write(std::vector<std::byte>* bytes, const FormatResult& value) {
        Write(bytes, value.newContent);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FormatResult, newContent)
};

FormatResult Format(Modules* modules, const FormatParams& params);
