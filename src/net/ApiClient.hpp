#pragma once

#include <filesystem>
#include <string>

#include "app/State.hpp"

namespace activator {

struct LoginResponse {
  bool ok{false};
  std::string token;
  std::string userId;
  std::string username;
  std::string plan;
  std::string expiresAt;
  std::string error;
};

struct VerifyResponse {
  bool ok{false};
  bool allowed{false};
  bool boundToThisPc{true};
  std::string cooldownUntil;
  std::string error;
};

struct BuildRequestResponse {
  bool ok{false};
  std::string buildId;
  std::string downloadUrl;
  std::string sha256;
  std::string cooldownUntil;
  std::string issuedAt;
  std::string expiresAt;
  std::string error;
};

struct OpResult {
  bool ok{false};
  std::string error;
};

class ApiClient {
 public:
  LoginResponse ValidateKeyAuthLicense(const std::string& license);
  VerifyResponse VerifyEntitlement(const std::string& baseUrl, const std::string& token,
                                   const std::string& hwidHash, const std::string& appVersion);
  BuildRequestResponse RequestBuild(const std::string& baseUrl, const std::string& token,
                                    const std::string& hwidHash, const std::string& channel,
                                    const std::string& extensionVersion);
  BuildRequestResponse BuildStatus(const std::string& baseUrl, const std::string& token,
                                   const std::string& buildId);
  OpResult DownloadFile(const std::string& url, const std::filesystem::path& outFile,
                        DownloadProgress& progress);
  bool VerifyDownloadedFile(const std::filesystem::path& filePath,
                            const std::string& expectedSha256);
};

}  // namespace activator
