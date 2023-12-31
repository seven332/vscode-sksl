#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "data.h"
#include "module.h"
#include "simple_codec.h"

struct GetTokenParams {
    std::string file;

    friend std::size_t Read(std::span<const std::byte> bytes, std::size_t offset, GetTokenParams* value) {
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

struct GetTokenRangeParams {
    std::string file;
    SkSLRange range;

    friend std::size_t Read(std::span<const std::byte> bytes, std::size_t offset, GetTokenRangeParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        read += Read(bytes, offset + read, &value->range);
        return read;
    }
};

using GetTokenRangeResult = GetTokenResult;

GetTokenRangeResult GetTokenRange(Modules* modules, const GetTokenRangeParams& params);
