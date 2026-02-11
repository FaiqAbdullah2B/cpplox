#pragma once
#include <vector>
#include <filesystem>
#include <cstddef>
#include <fstream>

namespace fs = std::filesystem;

class Files {
public:
    std::vector<std::byte> readAllBytes(const fs::path& path);
private:

};