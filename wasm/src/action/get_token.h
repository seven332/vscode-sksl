#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "data.h"
#include "module.h"
#include "simple_codec.h"

struct GetTokenParams {
    std::string file;

    friend std::size_t Read(std::span<std::byte> bytes, std::size_t offset, GetTokenParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        return read;
    }
};

struct GetTokenResult {
    std::vector<SkSLToken> tokens;

    friend void Write(std::vector<std::byte>* bytes, const GetTokenResult& value) {
        Write(bytes, value.tokens);
    }
};

GetTokenResult GetToken(Modules* modules, const GetTokenParams& params);
