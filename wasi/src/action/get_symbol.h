#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "data.h"
#include "module.h"

struct GetSymbolParams {
    std::string file;

    friend std::size_t Read(const std::vector<std::byte>& bytes, std::size_t offset, GetSymbolParams* value) {
        std::size_t read = 0;
        read += Read(bytes, offset + read, &value->file);
        return read;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetSymbolParams, file)
};

struct GetSymbolResult {
    std::vector<SkSLSymbol> symbols;

    friend void Write(std::vector<std::byte>* bytes, const GetSymbolResult& value) {
        Write(bytes, value.symbols);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetSymbolResult, symbols)
};

GetSymbolResult GetSymbol(Modules* modules, const GetSymbolParams& params);
