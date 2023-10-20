#pragma once

#include <cstddef>
#include <string>
#include <vector>

#pragma mark - Write

void Write(std::vector<std::byte>* bytes, bool value);
void Write(std::vector<std::byte>* bytes, int value);
void Write(std::vector<std::byte>* bytes, const std::string& value);

template<class T>
void Write(std::vector<std::byte>* bytes, const std::vector<T>& value) {
    Write(bytes, static_cast<int>(value.size()));
    for (const auto& element : value) {
        Write(bytes, element);
    }
}

#pragma mark - Read

std::size_t Read(const std::vector<std::byte>& bytes, std::size_t offset, bool* value);
std::size_t Read(const std::vector<std::byte>& bytes, std::size_t offset, int* value);
std::size_t Read(const std::vector<std::byte>& bytes, std::size_t offset, std::string* value);

template<class T>
std::size_t Read(const std::vector<std::byte>& bytes, std::size_t offset, std::vector<T>* value) {
    std::size_t read = 0;
    int size = 0;
    read += Read(bytes, offset + read, &size);
    value->resize(size);
    for (auto& element : *value) {
        read += Read(bytes, offset + read, &element);
    }
    return read;
}
