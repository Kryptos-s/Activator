#pragma once

#include "app/State.hpp"
#include "net/ApiClient.hpp"

namespace activator {

class DownloadScreen {
 public:
  explicit DownloadScreen(ApiClient& apiClient);
  void Render(AppState& state);

 private:
  ApiClient& api_;
};

}  // namespace activator
