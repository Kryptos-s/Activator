#include "security/RateLimit.hpp"

namespace activator::security {

BindingPolicy ResolveBindingPolicy(bool boundToThisPc, const std::string& cooldownUntil) {
  return {.boundToThisPc = boundToThisPc,
          .rebindAllowed = boundToThisPc || cooldownUntil.empty(),
          .cooldownUntil = cooldownUntil};
}

}  // namespace activator::security
