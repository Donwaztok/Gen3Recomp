#include "file_bytes.hpp"

#include "sha1.hpp"

#include <fstream>
#include <stdexcept>

std::vector<std::uint8_t> read_file_bytes(const std::filesystem::path& path) {
    std::ifstream in{path, std::ios::binary};
    if (!in) {
        throw std::runtime_error("failed to read file: " + path.string());
    }
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

std::string sha1_file(const std::filesystem::path& path) {
    return sha1_bytes_hex(read_file_bytes(path));
}
