#include "util/Strings.hpp"

#include <algorithm>
#include <cctype>

namespace activator::util {

std::string Trim(const std::string& value) {
  auto start = value.find_first_not_of(" \t\n\r");
  if (start == std::string::npos) {
    return {};
  }
  auto end = value.find_last_not_of(" \t\n\r");
  return value.substr(start, end - start + 1);
}

std::string ToLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return value;
}

}  // namespace activator::util
