#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>

std::string sha1_hex(std::span<const std::uint8_t> data);
std::string sha1_bytes_hex(const std::vector<std::uint8_t>& data);
