#pragma once

#include <span>
#include <string>

#include "data.h"
#include "module.h"
#include "simple_codec.h"

struct HoverParams {
    std::string file;
    int position;

    friend std::size_t Read(std::span<std::byte> bytes, std::size_t offset, HoverParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        read += Read(bytes, offset + read, &value->position);
        return read;
    }
};

struct HoverResult {
    bool found;
    bool markdown;
    std::string content;
    SkSLRange range;

    friend void Write(std::vector<std::byte>* bytes, const HoverResult& value) {
        Write(bytes, value.found);
        Write(bytes, value.markdown);
        Write(bytes, value.content);
        Write(bytes, value.range);
    }
};

HoverResult Hover(Modules* modules, const HoverParams& params);
