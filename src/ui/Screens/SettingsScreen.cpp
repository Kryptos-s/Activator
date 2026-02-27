#include "ui/Screens/SettingsScreen.hpp"

#include <imgui.h>

#include "security/Hwid.hpp"
#include "util/Files.hpp"

namespace activator {

SettingsScreen::SettingsScreen(SecureStore& secureStore) : secureStore_(secureStore) {}

void SettingsScreen::Render(AppState& state) {
  ImGui::TextUnformatted("Settings");
  ImGui::Separator();

  ImGui::Checkbox("Enable debug logging", &state.debugLogging);

  if (ImGui::Button("Copy diagnostics")) {
    state.diagnostics = "app=1.0.0\nos=windows\n";
    for (const auto& line : state.recentLogs) {
      state.diagnostics += line + "\n";
    }
    ImGui::SetClipboardText(state.diagnostics.c_str());
    state.statusMessage = "Diagnostics copied.";
  }

  if (ImGui::Button("Open log folder")) {
    util::OpenInFileExplorer(util::LogDirectory());
  }

  if (ImGui::Button("Logout and wipe local session")) {
    secureStore_.DeleteSecret("session_token");
    secureStore_.DeleteSecret("license_enc");
    util::ClearDownloads();
    state = AppState{};
    state.hwidHash = security::GetHwidHash();
    state.hwidShort = state.hwidHash.substr(0, 8);
    state.statusMessage = "Session wiped.";
  }

  if (!state.statusMessage.empty()) {
    ImGui::Spacing();
    ImGui::TextWrapped("%s", state.statusMessage.c_str());
  }
}

}  // namespace activator
