#include "util/Files.hpp"

#include <cstdlib>
#include <string>
#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#include <shellapi.h>
#endif

namespace activator::util {

namespace {
std::filesystem::path Ensure(const std::filesystem::path& path) {
  std::filesystem::create_directories(path);
  return path;
}

std::string ReadEnv(const char* key) {
#ifdef _WIN32
  char* buffer = nullptr;
  std::size_t len = 0;
  if (_dupenv_s(&buffer, &len, key) != 0 || buffer == nullptr) {
    return {};
  }
  std::string value(buffer);
  std::free(buffer);
  return value;
#else
  const char* value = std::getenv(key);
  return value != nullptr ? std::string(value) : std::string{};
#endif
}
}  // namespace

std::filesystem::path AppDataDirectory() {
#ifdef _WIN32
  const auto appData = ReadEnv("APPDATA");
  return Ensure(std::filesystem::path(appData.empty() ? "." : appData) / "Activator");
#else
  return Ensure(std::filesystem::path(".") / ".activator");
#endif
}

std::filesystem::path ConfigDirectory() { return Ensure(AppDataDirectory()); }
std::filesystem::path DownloadDirectory() { return Ensure(AppDataDirectory() / "downloads"); }
std::filesystem::path LogDirectory() { return Ensure(AppDataDirectory() / "logs"); }
std::filesystem::path DownloadPath(const std::string& child) { return DownloadDirectory() / child; }

void OpenInFileExplorer(const std::filesystem::path& path) {
#ifdef _WIN32
  ShellExecuteA(nullptr, "open", path.string().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
  (void)path;
#endif
}

void ClearDownloads() {
  std::error_code ec;
  std::filesystem::remove_all(DownloadDirectory(), ec);
  std::filesystem::create_directories(DownloadDirectory(), ec);
}

}  // namespace activator::util
