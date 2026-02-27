#include "app/App.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <chrono>
#include <cstdlib>
#include <string>

#include "security/Hwid.hpp"
#include "ui/Screens/DashboardScreen.hpp"
#include "ui/Screens/DownloadScreen.hpp"
#include "ui/Screens/LoginScreen.hpp"
#include "ui/Screens/SettingsScreen.hpp"
#include "ui/Theme.hpp"
#include "ui/Widgets/Toasts.hpp"

namespace activator {

namespace {
constexpr int kDefaultWidth = 1100;
constexpr int kDefaultHeight = 700;

std::string ReadEnv(const char* key) {
#ifdef _WIN32
  char* buffer = nullptr;
  std::size_t len = 0;
  if (_dupenv_s(&buffer, &len, key) != 0 || buffer == nullptr) {
    return {};
  }
  std::string value(buffer);
  std::free(buffer);
  return value;
#else
  const char* value = std::getenv(key);
  return value != nullptr ? std::string(value) : std::string{};
#endif
}
}  // namespace

App::App() = default;

App::~App() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }
}

bool App::InitWindow() {
  if (!glfwInit()) {
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window_ = glfwCreateWindow(kDefaultWidth, kDefaultHeight, "Activator", nullptr, nullptr);
  if (window_ == nullptr) {
    return false;
  }

  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);
  glfwSetWindowSizeLimits(window_, 900, 580, GLFW_DONT_CARE, GLFW_DONT_CARE);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init("#version 330");
  ui::ApplyTheme();

  return true;
}

void App::InitState() {
  const auto backend = ReadEnv("ACTIVATOR_BACKEND_URL");
  if (!backend.empty()) {
    state_.backendUrl = backend;
  } else {
    state_.backendUrl = "https://localhost:7443";
  }

  state_.hwidHash = security::GetHwidHash();
  state_.hwidShort = state_.hwidHash.substr(0, 8);

  const auto token = secureStore_.LoadSecret("session_token");
  if (token.has_value()) {
    state_.authToken = token.value();
    state_.authState = AuthState::LoggedIn;
    state_.view = View::Dashboard;
  }

  login_ = std::make_unique<LoginScreen>(api_, secureStore_);
  dashboard_ = std::make_unique<DashboardScreen>(api_);
  downloads_ = std::make_unique<DownloadScreen>(api_);
  settings_ = std::make_unique<SettingsScreen>(secureStore_);
}

int App::Run() {
  if (!InitWindow()) {
    return 1;
  }
  InitState();

  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    Render();

    ImGui::Render();
    int displayW = 0;
    int displayH = 0;
    glfwGetFramebufferSize(window_, &displayW, &displayH);
    glViewport(0, 0, displayW, displayH);
    glClearColor(0.07F, 0.08F, 0.1F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window_);
  }

  return 0;
}

void App::RenderSidebar() {
  ImGui::BeginChild("Sidebar", ImVec2(220, 0), true);
  ImGui::TextUnformatted("Activator");
  ImGui::Separator();
  if (ImGui::Selectable("Login", state_.view == View::Login)) {
    state_.view = View::Login;
  }
  if (ImGui::Selectable("Dashboard", state_.view == View::Dashboard)) {
    state_.view = View::Dashboard;
  }
  if (ImGui::Selectable("Downloads", state_.view == View::Downloads)) {
    state_.view = View::Downloads;
  }
  if (ImGui::Selectable("Settings", state_.view == View::Settings)) {
    state_.view = View::Settings;
  }
  ImGui::Spacing();
  ImGui::Text("HWID: %s", state_.hwidShort.c_str());
  ImGui::EndChild();
}

void App::Render() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
  ImGui::Begin("Root", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);

  RenderSidebar();
  ImGui::SameLine();

  ImGui::BeginChild("MainView", ImVec2(0, 0), false);
  switch (state_.view) {
    case View::Login:
      login_->Render(state_);
      break;
    case View::Dashboard:
      dashboard_->Render(state_);
      break;
    case View::Downloads:
      downloads_->Render(state_);
      break;
    case View::Settings:
      settings_->Render(state_);
      break;
  }
  ImGui::EndChild();

  TickToasts();
  ui::DrawToasts(state_.toasts);

  ImGui::End();
}

void App::PushToast(ToastType type, const std::string& message) {
  state_.toasts.push_back(
      {.type = type,
       .text = message,
       .expiresAt = std::chrono::steady_clock::now() + std::chrono::seconds(4)});
}

void App::TickToasts() {
  const auto now = std::chrono::steady_clock::now();
  while (!state_.toasts.empty() && state_.toasts.front().expiresAt <= now) {
    state_.toasts.erase(state_.toasts.begin());
  }
}

}  // namespace activator
