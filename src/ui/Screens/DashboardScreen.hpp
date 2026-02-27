#pragma once

#include "app/State.hpp"
#include "net/ApiClient.hpp"

namespace activator {

class DashboardScreen {
 public:
  explicit DashboardScreen(ApiClient& apiClient);
  void Render(AppState& state);

 private:
  ApiClient& api_;
};

}  // namespace activator
