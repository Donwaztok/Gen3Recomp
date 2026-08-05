#pragma once

#include <filesystem>
#include <string>

namespace gen3recomp {

std::filesystem::path user_data_dir();
std::filesystem::path recomp_cache_dir(const std::string& rom_sha1);
std::filesystem::path cartridge_save_path(const std::string& rom_sha1);

}
