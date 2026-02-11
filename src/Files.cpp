#include "Files.h"

std::vector<std::byte> Files::readAllBytes(const fs::path &path) {

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open the file");
    }

    if (!fs::exists(path)) {
        throw std::runtime_error("Could not find file.");
    }

    auto size = file.tellg();

    if (size == -1) {
        throw std::runtime_error("Could not determine file size.");
    }

    std::vector<std::byte> buffer(size);

    file.seekg(0, std::ios::beg);

    // Reading into buffer
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Failed to read the expected number of bytes.");
    }

    return buffer;
}