#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <span>
#include <string>
#include <vector>

#include "action/close.h"
#include "action/format.h"
#include "action/get_symbol.h"
#include "action/get_token.h"
#include "action/update.h"
#include "module.h"

static Modules modules;
static std::vector<std::byte> params_bytes;
static std::vector<std::byte> result_bytes;

extern "C" {

void SetParamsSize(std::size_t size) {
    params_bytes.resize(size);
}

void* GetParamsPtr() {
    return params_bytes.data();
}

void* GetResultPtr() {
    return result_bytes.data();
}

std::size_t GetResultSize() {
    return result_bytes.size();
}

#define ACTION(Type, Name)                               \
    void Type() {                                        \
        Type##Params params;                             \
        Read(params_bytes, 0, &params);                  \
                                                         \
        auto file = params.file;                         \
        std::cout << Name " start: " << file << '\n';    \
        auto result = Type(&modules, std::move(params)); \
        std::cout << Name " end:   " << file << '\n';    \
                                                         \
        result_bytes.clear();                            \
        Write(&result_bytes, result);                    \
    }

ACTION(Update, "update")
ACTION(Close, "close")
ACTION(GetSymbol, "get_symbol")
ACTION(Format, "format")
ACTION(GetToken, "get_token")
ACTION(GetTokenRange, "get_token_range")

#undef ACTION
}
