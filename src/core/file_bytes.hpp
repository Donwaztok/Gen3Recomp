#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

std::vector<std::uint8_t> read_file_bytes(const std::filesystem::path& path);
std::string sha1_file(const std::filesystem::path& path);
