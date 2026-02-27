#include "net/ApiClient.hpp"

#include <nlohmann/json.hpp>

#include <filesystem>

#include "net/Http.hpp"
#include "security/Crypto.hpp"

namespace activator {
using json = nlohmann::json;

LoginResponse ApiClient::ValidateKeyAuthLicense(const std::string& license) {
  // Placeholder adapter. Replace with your existing KeyAuth integration call.
  if (license.size() < 8) {
    return {.ok = false, .error = "INVALID_LICENSE"};
  }

  return {.ok = true,
          .token = "keyauth-session-token",
          .userId = "user-123",
          .username = "licensed-user",
          .plan = "pro",
          .expiresAt = "2099-01-01T00:00:00Z"};
}

VerifyResponse ApiClient::VerifyEntitlement(const std::string& baseUrl, const std::string& token,
                                            const std::string& hwidHash,
                                            const std::string& appVersion) {
  const auto body = json{{"hwidHash", hwidHash}, {"appVersion", appVersion}}.dump();
  const auto res = net::PostJson(baseUrl + "/v1/entitlements/verify", body,
                                 {{"Authorization", "Bearer " + token},
                                  {"Content-Type", "application/json"}});
  if (!res.error.empty()) {
    return {.ok = false, .error = "NETWORK"};
  }
  if (res.status < 200 || res.status >= 300) {
    return {.ok = false, .error = "NOT_ENTITLED"};
  }

  auto payload = json::parse(res.body, nullptr, false);
  if (payload.is_discarded()) {
    return {.ok = false, .error = "NETWORK"};
  }

  return {.ok = payload.value("ok", false),
          .allowed = payload.value("allowed", false),
          .boundToThisPc = payload.value("boundToThisPc", true),
          .cooldownUntil = payload.value("cooldownUntil", "")};
}

BuildRequestResponse ApiClient::RequestBuild(const std::string& baseUrl, const std::string& token,
                                             const std::string& hwidHash,
                                             const std::string& channel,
                                             const std::string& extensionVersion) {
  const auto body =
      json{{"hwidHash", hwidHash}, {"channel", channel}, {"extensionVersion", extensionVersion}}
          .dump();
  const auto res = net::PostJson(baseUrl + "/v1/build/request", body,
                                 {{"Authorization", "Bearer " + token},
                                  {"Content-Type", "application/json"}});
  if (!res.error.empty()) {
    return {.ok = false, .error = "NETWORK"};
  }
  if (res.status < 200 || res.status >= 300) {
    return {.ok = false, .error = "COOLDOWN"};
  }

  auto payload = json::parse(res.body, nullptr, false);
  if (payload.is_discarded()) {
    return {.ok = false, .error = "BUILD_FAILED"};
  }

  return {.ok = payload.value("ok", false),
          .buildId = payload.value("buildId", ""),
          .downloadUrl = payload.value("downloadUrl", ""),
          .sha256 = payload.value("sha256", ""),
          .cooldownUntil = payload.value("cooldownUntil", ""),
          .issuedAt = payload.value("issuedAt", ""),
          .expiresAt = payload.value("expiresAt", "")};
}

BuildRequestResponse ApiClient::BuildStatus(const std::string& baseUrl, const std::string& token,
                                            const std::string& buildId) {
  const auto res = net::Get(baseUrl + "/v1/build/status?buildId=" + buildId,
                            {{"Authorization", "Bearer " + token}});
  if (!res.error.empty()) {
    return {.ok = false, .error = "NETWORK"};
  }

  auto payload = json::parse(res.body, nullptr, false);
  if (payload.is_discarded()) {
    return {.ok = false, .error = "BUILD_FAILED"};
  }

  return {.ok = payload.value("ok", false),
          .buildId = buildId,
          .downloadUrl = payload.value("downloadUrl", ""),
          .sha256 = payload.value("sha256", "")};
}

OpResult ApiClient::DownloadFile(const std::string& url, const std::filesystem::path& outFile,
                                 DownloadProgress& progress) {
  std::string err;
  bool ok = net::Download(url, outFile.string(), progress.downloadedBytes, progress.totalBytes,
                          progress.bytesPerSecond, progress.etaSeconds, err);
  return {.ok = ok, .error = err};
}

bool ApiClient::VerifyDownloadedFile(const std::filesystem::path& filePath,
                                     const std::string& expectedSha256) {
  return security::Sha256FileHex(filePath) == expectedSha256;
}

}  // namespace activator
