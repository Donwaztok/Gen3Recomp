#pragma once

#include <filesystem>
#include <string>

namespace gen3recomp {

std::filesystem::path user_data_dir();
std::filesystem::path recomp_cache_dir(const std::string& rom_sha1);
std::filesystem::path cartridge_save_path(const std::string& rom_sha1);

// Full-cart AOT shared library lives under:
//   <user_data>/cart_aot/<sha1>/<abi>/libcart.so
// Abi segment tracks host OS/arch so a pin bump can invalidate old blobs.
std::string cart_artifact_abi_tag();
std::filesystem::path cart_artifact_dir(const std::string& rom_sha1);
std::filesystem::path cart_artifact_path(const std::string& rom_sha1);

}
