#pragma once

#include <string>

#include "data.h"
#include "module.h"

struct DefinitionParams {
    std::string file;
    int position;

    friend std::size_t Read(std::span<std::byte> bytes, std::size_t offset, DefinitionParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        read += Read(bytes, offset + read, &value->position);
        return read;
    }
};

struct DefinitionResult {
    bool found = false;
    SkSLRange range;

    friend void Write(std::vector<std::byte>* bytes, const DefinitionResult& value) {
        Write(bytes, value.found);
        Write(bytes, value.range);
    }
};

DefinitionResult Definition(Modules* modules, const DefinitionParams& params);
