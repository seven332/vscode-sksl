#pragma once

#include <string>

#include "data.h"
#include "module.h"
#include "simple_codec.h"

struct UpdateParams {
    std::string file;
    std::string content;
    std::string kind;

    friend std::size_t Read(const std::vector<std::byte>& bytes, std::size_t offset, UpdateParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        read += Read(bytes, offset + read, &value->content);
        read += Read(bytes, offset + read, &value->kind);
        return read;
    }
};

struct UpdateResult {
    bool succeed = false;
    std::vector<SkSLError> errors;

    friend void Write(std::vector<std::byte>* bytes, const UpdateResult& value) {
        Write(bytes, value.succeed);
        Write(bytes, value.errors);
    }
};

UpdateResult Update(Modules* modules, const UpdateParams& params);
