#include "net/Http.hpp"

#include <cpr/cpr.h>

#include <chrono>
#include <fstream>

namespace activator::net {

HttpResponse PostJson(const std::string& url, const std::string& jsonBody,
                      const std::map<std::string, std::string>& headers) {
  cpr::Header h;
  for (const auto& [k, v] : headers) {
    h[k] = v;
  }

  auto response = cpr::Post(cpr::Url{url}, h, cpr::Body{jsonBody}, cpr::VerifySsl{true});
  return {.status = response.status_code, .body = response.text, .error = response.error.message};
}

HttpResponse Get(const std::string& url, const std::map<std::string, std::string>& headers) {
  cpr::Header h;
  for (const auto& [k, v] : headers) {
    h[k] = v;
  }

  auto response = cpr::Get(cpr::Url{url}, h, cpr::VerifySsl{true});
  return {.status = response.status_code, .body = response.text, .error = response.error.message};
}

bool Download(const std::string& url, const std::string& outputPath, std::size_t& downloaded,
              std::size_t& total, double& speedBytesPerSec, int& etaSeconds,
              std::string& errorOut) {
  std::ofstream out(outputPath, std::ios::binary);
  if (!out.good()) {
    errorOut = "Unable to open output file";
    return false;
  }

  const auto start = std::chrono::steady_clock::now();
  downloaded = 0;
  total = 0;
  auto response = cpr::Download(out, cpr::Url{url}, cpr::VerifySsl{true},
                                cpr::ProgressCallback{[&](cpr_off_t dlTotal, cpr_off_t dlNow,
                                                          cpr_off_t, cpr_off_t) {
                                  total = static_cast<std::size_t>(dlTotal);
                                  downloaded = static_cast<std::size_t>(dlNow);
                                  const auto elapsed = std::chrono::duration<double>(
                                      std::chrono::steady_clock::now() - start);
                                  speedBytesPerSec =
                                      elapsed.count() > 0 ? downloaded / elapsed.count() : 0.0;
                                  if (speedBytesPerSec > 0 && total > downloaded) {
                                    etaSeconds = static_cast<int>((total - downloaded) /
                                                                  speedBytesPerSec);
                                  }
                                  return true;
                                }});

  if (response.error.code != cpr::ErrorCode::OK) {
    errorOut = response.error.message;
    return false;
  }

  return response.status_code >= 200 && response.status_code < 300;
}

}  // namespace activator::net
