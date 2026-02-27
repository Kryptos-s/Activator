#pragma once

#include <optional>
#include <string>

namespace activator {

class SecureStore {
 public:
  bool SaveSecret(const std::string& key, const std::string& value) const;
  std::optional<std::string> LoadSecret(const std::string& key) const;
  bool DeleteSecret(const std::string& key) const;
};

}  // namespace activator
