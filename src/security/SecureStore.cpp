#include "security/SecureStore.hpp"

#include <filesystem>
#include <fstream>
#include <vector>

#include "util/Files.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <dpapi.h>
#endif

namespace activator {

namespace {
std::filesystem::path SecretPath(const std::string& key) {
  return util::ConfigDirectory() / (key + ".bin");
}
}  // namespace

bool SecureStore::SaveSecret(const std::string& key, const std::string& value) const {
#ifdef _WIN32
  DATA_BLOB input{static_cast<DWORD>(value.size()),
                  reinterpret_cast<BYTE*>(const_cast<char*>(value.data()))};
  DATA_BLOB output{};
  if (!CryptProtectData(&input, L"Activator", nullptr, nullptr, nullptr, 0, &output)) {
    return false;
  }

  std::ofstream file(SecretPath(key), std::ios::binary);
  file.write(reinterpret_cast<char*>(output.pbData), static_cast<std::streamsize>(output.cbData));
  LocalFree(output.pbData);
  return file.good();
#else
  (void)key;
  (void)value;
  return false;
#endif
}

std::optional<std::string> SecureStore::LoadSecret(const std::string& key) const {
#ifdef _WIN32
  std::ifstream file(SecretPath(key), std::ios::binary);
  if (!file.good()) {
    return std::nullopt;
  }

  std::vector<char> encrypted((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  if (encrypted.empty()) {
    return std::nullopt;
  }

  DATA_BLOB input{static_cast<DWORD>(encrypted.size()), reinterpret_cast<BYTE*>(encrypted.data())};
  DATA_BLOB output{};
  if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, 0, &output)) {
    return std::nullopt;
  }

  std::string result(reinterpret_cast<char*>(output.pbData), output.cbData);
  LocalFree(output.pbData);
  return result;
#else
  (void)key;
  return std::nullopt;
#endif
}

bool SecureStore::DeleteSecret(const std::string& key) const {
  return std::filesystem::remove(SecretPath(key));
}

}  // namespace activator
