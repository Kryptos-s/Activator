#include "ui/Screens/LoginScreen.hpp"

#include <imgui.h>

#include <cstdio>

#include "security/Crypto.hpp"

namespace activator {

LoginScreen::LoginScreen(ApiClient& apiClient, SecureStore& secureStore)
    : api_(apiClient), secureStore_(secureStore) {}

void LoginScreen::Render(AppState& state) {
  ImGui::TextUnformatted("License Login");
  ImGui::Separator();
  char licenseBuffer[128]{};
  std::snprintf(licenseBuffer, sizeof(licenseBuffer), "%s", licenseInput_.c_str());
  if (ImGui::InputTextWithHint("##license", "Enter your license key", licenseBuffer, sizeof(licenseBuffer))) {
    licenseInput_ = licenseBuffer;
  }

  if (ImGui::Button("Sign in", ImVec2(180, 0))) {
    state.authState = AuthState::LoggingIn;
    auto login = api_.ValidateKeyAuthLicense(licenseInput_);
    if (!login.ok) {
      state.authState = AuthState::Error;
      state.statusMessage = login.error;
      return;
    }

    state.authToken = login.token;
    state.identity = Identity{.userId = login.userId,
                              .username = login.username,
                              .plan = login.plan,
                              .expiresAt = login.expiresAt};
    state.maskedLicense = security::MaskLicense(licenseInput_);

    auto verify = api_.VerifyEntitlement(state.backendUrl, state.authToken, state.hwidHash, "1.0.0");
    if (!verify.ok || !verify.allowed) {
      state.authState = AuthState::Error;
      state.statusMessage = verify.error;
      return;
    }

    state.boundToThisPc = verify.boundToThisPc;
    if (!verify.cooldownUntil.empty()) {
      state.buildState = BuildState::Cooldown;
    }

    secureStore_.SaveSecret("session_token", state.authToken);
    secureStore_.SaveSecret("license_enc", security::Sha256Hex(licenseInput_));
    state.authState = AuthState::LoggedIn;
    state.view = View::Dashboard;
    state.statusMessage = "Login successful";
  }

  if (!state.statusMessage.empty()) {
    ImGui::Spacing();
    ImGui::TextWrapped("%s", state.statusMessage.c_str());
  }
}

}  // namespace activator
