#pragma once

#include <string>

#include "data.h"
#include "module.h"
#include "simple_codec.h"

struct UpdateParams {
    std::string file;
    std::string content;

    friend std::size_t Read(std::span<std::byte> bytes, std::size_t offset, UpdateParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        read += Read(bytes, offset + read, &value->content);
        return read;
    }
};

struct UpdateResult {
    bool succeed = false;
    std::vector<SkSLDiagnostic> diagnostics;

    friend void Write(std::vector<std::byte>* bytes, const UpdateResult& value) {
        Write(bytes, value.succeed);
        Write(bytes, value.diagnostics);
    }
};

UpdateResult Update(Modules* modules, UpdateParams params);
