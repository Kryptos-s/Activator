#pragma once

#include <map>
#include <string>

namespace activator::net {

struct HttpResponse {
  int status{0};
  std::string body;
  std::string error;
};

HttpResponse PostJson(const std::string& url, const std::string& jsonBody,
                      const std::map<std::string, std::string>& headers);
HttpResponse Get(const std::string& url, const std::map<std::string, std::string>& headers);
bool Download(const std::string& url, const std::string& outputPath, std::size_t& downloaded,
              std::size_t& total, double& speedBytesPerSec, int& etaSeconds,
              std::string& errorOut);

}  // namespace activator::net
