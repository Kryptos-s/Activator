#pragma once

#include <string>

#include "app/State.hpp"
#include "net/ApiClient.hpp"
#include "security/SecureStore.hpp"

namespace activator {

class LoginScreen {
 public:
  LoginScreen(ApiClient& apiClient, SecureStore& secureStore);
  void Render(AppState& state);

 private:
  ApiClient& api_;
  SecureStore& secureStore_;
  std::string licenseInput_;
};

}  // namespace activator
