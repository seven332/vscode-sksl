#pragma once

#include <string>
#include <vector>

#include "data.h"
#include "simple_codec.h"

struct RunParams {
    std::string source;
    std::vector<std::string> values;

    friend std::size_t Read(std::span<const std::byte> bytes, std::size_t offset, RunParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->source);
        read += Read(bytes, offset + read, &value->values);
        return read;
    }
};

struct RunResult {
    bool succeed = false;
    SkSLColor color {};

    friend void Write(std::vector<std::byte>* bytes, const RunResult& value) {
        Write(bytes, value.succeed);
        Write(bytes, value.color);
    }
};

RunResult Run(RunParams params);
