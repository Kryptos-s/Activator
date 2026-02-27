#pragma once

#include "app/State.hpp"
#include "security/SecureStore.hpp"

namespace activator {

class SettingsScreen {
 public:
  explicit SettingsScreen(SecureStore& secureStore);
  void Render(AppState& state);

 private:
  SecureStore& secureStore_;
};

}  // namespace activator
