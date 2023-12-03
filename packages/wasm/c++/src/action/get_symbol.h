#pragma once

#include <string>

#include "data.h"
#include "module.h"

struct GetSymbolParams {
    std::string file;

    friend std::size_t Read(std::span<const std::byte> bytes, std::size_t offset, GetSymbolParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        return read;
    }
};

struct GetSymbolResult {
    std::vector<SkSLSymbol> symbols;

    friend void Write(std::vector<std::byte>* bytes, const GetSymbolResult& value) {
        Write(bytes, value.symbols);
    }
};

GetSymbolResult GetSymbol(Modules* modules, const GetSymbolParams& params);
