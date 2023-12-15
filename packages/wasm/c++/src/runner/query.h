#pragma once

#include <string>
#include <vector>

#include "data.h"
#include "simple_codec.h"

struct QueryParams {
    std::string source;

    friend std::size_t Read(std::span<const std::byte> bytes, std::size_t offset, QueryParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->source);
        return read;
    }
};

struct QueryResult {
    bool succeed = false;
    std::string kind;
    std::vector<SkSLUniform> uniforms;

    friend void Write(std::vector<std::byte>* bytes, const QueryResult& value) {
        Write(bytes, value.succeed);
        Write(bytes, value.kind);
        Write(bytes, value.uniforms);
    }
};

QueryResult Query(QueryParams params);
