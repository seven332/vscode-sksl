#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "module.h"
#include "simple_codec.h"

struct CloseParams {
    std::string file;

    friend std::size_t Read(const std::vector<std::byte>& bytes, std::size_t offset, CloseParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        return read;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CloseParams, file)
};

struct CloseResult {
    bool succeed = false;

    friend void Write(std::vector<std::byte>* bytes, const CloseResult& value) {
        Write(bytes, value.succeed);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CloseResult, succeed)
};

CloseResult Close(Modules* modules, const CloseParams& params);
