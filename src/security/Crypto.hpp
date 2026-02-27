#pragma once

#include <filesystem>
#include <string>

namespace activator::security {
std::string Sha256Hex(const std::string& input);
std::string Sha256FileHex(const std::filesystem::path& filePath);
std::string MaskLicense(const std::string& license);
}
