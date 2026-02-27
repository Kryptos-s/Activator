#include "security/Crypto.hpp"

#include <openssl/evp.h>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

namespace activator::security {

namespace {
std::string ToHex(const unsigned char* digest, unsigned int size) {
  std::ostringstream oss;
  for (unsigned int i = 0; i < size; ++i) {
    oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
  }
  return oss.str();
}
}  // namespace

std::string Sha256Hex(const std::string& input) {
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  unsigned char digest[EVP_MAX_MD_SIZE]{};
  unsigned int digestLen = 0;

  EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
  EVP_DigestUpdate(ctx, input.data(), input.size());
  EVP_DigestFinal_ex(ctx, digest, &digestLen);
  EVP_MD_CTX_free(ctx);

  return ToHex(digest, digestLen);
}

std::string Sha256FileHex(const std::filesystem::path& filePath) {
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  unsigned char digest[EVP_MAX_MD_SIZE]{};
  unsigned int digestLen = 0;
  std::vector<char> buffer(4096);

  std::ifstream file(filePath, std::ios::binary);
  if (!file.good()) {
    return {};
  }

  EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
  while (file.good()) {
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    if (file.gcount() > 0) {
      EVP_DigestUpdate(ctx, buffer.data(), static_cast<size_t>(file.gcount()));
    }
  }

  EVP_DigestFinal_ex(ctx, digest, &digestLen);
  EVP_MD_CTX_free(ctx);
  return ToHex(digest, digestLen);
}

std::string MaskLicense(const std::string& license) {
  if (license.size() <= 4) {
    return "****";
  }
  return std::string(license.size() - 4, '*') + license.substr(license.size() - 4);
}

}  // namespace activator::security
