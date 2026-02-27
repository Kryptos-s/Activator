#pragma once

#include <filesystem>

namespace activator::util {
std::filesystem::path AppDataDirectory();
std::filesystem::path ConfigDirectory();
std::filesystem::path DownloadDirectory();
std::filesystem::path LogDirectory();
std::filesystem::path DownloadPath(const std::string& child);
void OpenInFileExplorer(const std::filesystem::path& path);
void ClearDownloads();
}  // namespace activator::util
