#pragma once

#include <string>

#include "module.h"
#include "simple_codec.h"

struct FormatParams {
    std::string file;

    friend std::size_t Read(std::span<std::byte> bytes, std::size_t offset, FormatParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        return read;
    }
};

struct FormatResult {
    std::string newContent;  // NOLINT

    friend void Write(std::vector<std::byte>* bytes, const FormatResult& value) {
        Write(bytes, value.newContent);
    }
};

FormatResult Format(Modules* modules, const FormatParams& params);
