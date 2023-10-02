#include <array>
#include <cstdint>
#include <iostream>
#include <istream>
#include <nlohmann/json.hpp>
#include <string>

#pragma mark - Update

struct UpdateParams {
    std::string file;
    std::string content;
    std::string kind;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UpdateParams, file, content, kind)
};

static void Update(const UpdateParams& params) {
    // TODO:
    std::cout << "update: " << params.file << std::endl;
}

#pragma mark - Close

struct CloseParams {
    std::string file;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CloseParams, file)
};

static void Close(const CloseParams& params) {
    // TODO:
    std::cout << "Close: " << params.file << std::endl;
}

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

static constexpr std::uint32_t Hash(const char* str) {
    constexpr std::uint32_t kInitialValue = 17;
    constexpr std::uint32_t kMultiplierValue = 37;
    std::uint32_t hash = kInitialValue;
    while (*str) {
        hash = hash * kMultiplierValue + *str;
        ++str;
    }
    return hash;
}

extern "C" {

#define CALL(Action)                    \
auto j = nlohmann::json::parse(params); \
auto params = j.get<Action##Params>();  \
Action(params);

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
        }
    }
    return 0;
}
}
