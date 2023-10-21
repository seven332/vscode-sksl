#include "simple_codec.h"

#include <cstddef>
#include <cstring>
#include <iostream>

#pragma mark - Write

void Write(std::vector<std::byte>* bytes, bool value) {
    bytes->push_back(value ? static_cast<std::byte>(1) : static_cast<std::byte>(0));
}

void Write(std::vector<std::byte>* bytes, int value) {
    const auto* ptr = reinterpret_cast<const std::byte*>(&value);
    bytes->push_back(*ptr);
    bytes->push_back(*(ptr + 1));
    bytes->push_back(*(ptr + 2));
    bytes->push_back(*(ptr + 3));
}

void Write(std::vector<std::byte>* bytes, const std::string& value) {
    Write(bytes, static_cast<int>(value.size()));
    for (auto c : value) {
        bytes->push_back(static_cast<std::byte>(c));
    }
}

#pragma mark - Read

static void CheckSize(std::span<std::byte> bytes, std::size_t at_least) {
    if (bytes.size() < at_least) {
        std::cerr << "CheckSize failed: " << bytes.size() << " -> " << at_least << '\n';
        std::abort();
    }
}

std::size_t Read(std::span<std::byte> bytes, std::size_t offset, bool* value) {
    CheckSize(bytes, offset + 1);
    *value = *(bytes.data() + offset) != static_cast<std::byte>(0);
    return 1;
}

std::size_t Read(std::span<std::byte> bytes, std::size_t offset, int* value) {
    CheckSize(bytes, offset + 4);
    std::memcpy(value, bytes.data() + offset, 4);
    return 4;
}

std::size_t Read(std::span<std::byte> bytes, std::size_t offset, std::string* value) {
    int size = 0;
    Read(bytes, offset, &size);

    CheckSize(bytes, offset + 4 + size);
    *value = std::string(size, '\0');
    std::memcpy(value->data(), bytes.data() + offset + 4, size);

    return 4 + size;
}
