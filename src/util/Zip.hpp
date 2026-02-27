#pragma once

#include <filesystem>

namespace activator::util {
bool UnzipArchive(const std::filesystem::path& archivePath, const std::filesystem::path& destination);
}
