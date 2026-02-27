#include "ui/Screens/DashboardScreen.hpp"

#include <imgui.h>

namespace activator {

DashboardScreen::DashboardScreen(ApiClient& apiClient) : api_(apiClient) {}

void DashboardScreen::Render(AppState& state) {
  ImGui::TextUnformatted("Dashboard");
  ImGui::Separator();

  if (!state.identity.has_value()) {
    ImGui::TextUnformatted("No active session.");
    return;
  }

  ImGui::Text("User: %s", state.identity->username.c_str());
  ImGui::Text("Plan: %s", state.identity->plan.c_str());
  ImGui::Text("Expires: %s", state.identity->expiresAt.c_str());
  ImGui::Text("License: %s", state.maskedLicense.c_str());
  ImGui::Text("HWID hash: %s...", state.hwidShort.c_str());
  ImGui::Text("Binding status: %s", state.boundToThisPc ? "Bound to this PC" : "Bound to another PC");

  if (ImGui::Button("Refresh entitlement")) {
    auto verify = api_.VerifyEntitlement(state.backendUrl, state.authToken, state.hwidHash, "1.0.0");
    state.boundToThisPc = verify.boundToThisPc;
    if (!verify.cooldownUntil.empty()) {
      state.statusMessage = "Cooldown until " + verify.cooldownUntil;
      state.buildState = BuildState::Cooldown;
    } else {
      state.statusMessage = "Entitlement refreshed";
    }
  }

  if (!state.statusMessage.empty()) {
    ImGui::Spacing();
    ImGui::TextWrapped("%s", state.statusMessage.c_str());
  }
}

}  // namespace activator
