#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <string>
#include <vector>

#include "action/close.h"
#include "action/format.h"
#include "action/get_symbol.h"
#include "action/get_token.h"
#include "action/update.h"
#include "hash.h"
#include "module.h"
#include "simple_codec.h"

static Modules modules;

#pragma mark - Main

static std::byte GetByte() {
    int c = std::cin.get();
    if (c == std::istream::traits_type::eof()) {
        std::cerr << "cin eof" << std::endl;
        std::abort();
    }
    return static_cast<std::byte>(c);
}

static std::uint32_t GetUint32() {
    std::uint32_t result = 0;
    for (auto i = 0; i < 4; ++i) {
        auto byte = GetByte();
        *(reinterpret_cast<std::byte*>(&result) + i) = byte;
    }
    return result;
}

static std::vector<std::byte> GetBytes() {
    std::size_t size = GetUint32();
    std::vector<std::byte> bytes;
    bytes.reserve(size);
    for (auto i = 0; i < size; ++i) {
        bytes.push_back(GetByte());
    }
    return bytes;
}

extern "C" {

#define CALL(Action)                                                                          \
    Action##Params params;                                                                    \
    Read(GetBytes(), 0, &params);                                                             \
    auto result = Action(&modules, std::move(params));                                        \
    std::vector<std::byte> buffer;                                                            \
    Write(&buffer, result);                                                                   \
    std::uint32_t size = buffer.size();                                                       \
    std::cout.write(reinterpret_cast<const std::istream::traits_type::char_type*>(&size), 4); \
    std::cout.write(                                                                          \
        reinterpret_cast<const std::istream::traits_type::char_type*>(buffer.data()),         \
        static_cast<std::streamsize>(buffer.size())                                           \
    );

int main(void) {
    while (true) {
        std::string url;
        Read(GetBytes(), 0, &url);
        switch (Hash(url.data())) {
        case Hash("sksl/update"): {
            CALL(Update)
            break;
        }
        case Hash("sksl/close"): {
            CALL(Close)
            break;
        }
        case Hash("sksl/get-symbol"): {
            CALL(GetSymbol)
            break;
        }
        case Hash("sksl/format"): {
            CALL(Format)
            break;
        }
        case Hash("sksl/get-token"): {
            CALL(GetToken)
            break;
        }
        default:
            std::cerr << "Abort: invalid url " << url << std::endl;
            std::abort();
        }
    }
    return 0;
}
}
