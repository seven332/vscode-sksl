#pragma once

#include <vector>

#include "data.h"
#include "module.h"

struct CompletionParams {
    std::string file;
    int position;

    friend std::size_t Read(std::span<const std::byte> bytes, std::size_t offset, CompletionParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        read += Read(bytes, offset + read, &value->position);
        return read;
    }
};

struct CompletionResult {
    std::vector<SkSLCompletion> items;

    friend void Write(std::vector<std::byte>* bytes, const CompletionResult& value) {
        Write(bytes, value.items);
    }
};

CompletionResult Completion(Modules* modules, const CompletionParams& params);
