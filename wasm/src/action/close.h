#pragma once

#include <string>

#include "module.h"
#include "simple_codec.h"

struct CloseParams {
    std::string file;

    friend std::size_t Read(std::span<std::byte> bytes, std::size_t offset, CloseParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        return read;
    }
};

struct CloseResult {
    bool succeed = false;

    friend void Write(std::vector<std::byte>* bytes, const CloseResult& value) {
        Write(bytes, value.succeed);
    }
};

CloseResult Close(Modules* modules, const CloseParams& params);
