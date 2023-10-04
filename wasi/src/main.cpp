#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <string>

#include "action/close.h"
#include "action/get_symbol.h"
#include "action/update.h"
#include "hash.h"
#include "modules.h"

static Modules modules;

#pragma mark - Main

static std::string GetLine() {
    constexpr std::uint32_t kNextLine = 10;
    std::string result;
    while (true) {
        int c = std::cin.get();
        if (c == kNextLine || c == std::istream::traits_type::eof()) {
            return result;
        } else {
            result += static_cast<char>(c);
        }
    }
}

extern "C" {

#define CALL(Action)                    \
auto j = nlohmann::json::parse(params); \
auto params = j.get<Action##Params>();  \
j = Action(&modules, params);           \
std::cout << j.dump() << std::endl;

int main(void) {
    while (true) {
        auto method = GetLine();
        auto params = GetLine();
        switch (Hash(method.data())) {
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
        default:
            std::cerr << "Abort: invalid method: " << method << std::endl;
            std::abort();
        }
    }
    return 0;
}
}
