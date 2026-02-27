#pragma once

#include <string>

namespace activator::security {

struct BindingPolicy {
  bool boundToThisPc{true};
  bool rebindAllowed{false};
  std::string cooldownUntil;
};

BindingPolicy ResolveBindingPolicy(bool boundToThisPc, const std::string& cooldownUntil);

}  // namespace activator::security
