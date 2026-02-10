#include "Files.h"

std::vector<std::byte> Files::readAllBytes(const fs::path &path) {
    if (!fs::exists(path)) {
        throw std::runtime_error("Could not find file.");
    }

    auto size = fs::file_size(path);

    std::vector<std::byte> buffer(size);

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open the file");
    }

    file.read(reinterpret_cast<char*>(buffer.data()), size);

    return buffer;
}