#pragma once

#include <memory>
#include <string>

#include "app/State.hpp"
#include "net/ApiClient.hpp"
#include "security/SecureStore.hpp"

struct GLFWwindow;

namespace activator {
class LoginScreen;
class DashboardScreen;
class DownloadScreen;
class SettingsScreen;

class App {
 public:
  App();
  ~App();

  int Run();

 private:
  bool InitWindow();
  void InitState();
  void Render();
  void RenderSidebar();
  void PushToast(ToastType type, const std::string& message);
  void TickToasts();

  GLFWwindow* window_{nullptr};
  AppState state_{};
  ApiClient api_;
  SecureStore secureStore_;
  std::unique_ptr<LoginScreen> login_;
  std::unique_ptr<DashboardScreen> dashboard_;
  std::unique_ptr<DownloadScreen> downloads_;
  std::unique_ptr<SettingsScreen> settings_;
};

}  // namespace activator
