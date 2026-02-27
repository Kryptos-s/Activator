#include "ui/Screens/DownloadScreen.hpp"

#include <imgui.h>

#include "util/Files.hpp"
#include "util/Zip.hpp"

namespace activator {

DownloadScreen::DownloadScreen(ApiClient& apiClient) : api_(apiClient) {}

void DownloadScreen::Render(AppState& state) {
  ImGui::TextUnformatted("Downloads");
  ImGui::Separator();

  if (ImGui::Button("Request custom build", ImVec2(220, 0))) {
    auto response = api_.RequestBuild(state.backendUrl, state.authToken, state.hwidHash, "stable", "1.0.0");
    if (!response.ok) {
      state.buildState = BuildState::Failed;
      state.statusMessage = response.error;
      return;
    }
    state.currentBuild = BuildInfo{.buildId = response.buildId,
                                   .downloadUrl = response.downloadUrl,
                                   .sha256 = response.sha256,
                                   .cooldownUntil = response.cooldownUntil,
                                   .expiresAt = response.expiresAt,
                                   .issuedAt = response.issuedAt};
    state.buildState = BuildState::Queued;
    state.statusMessage = "Build requested: " + response.buildId;
  }

  if (state.currentBuild.has_value() && ImGui::Button("Download & verify", ImVec2(220, 0))) {
    const auto zipPath = util::DownloadPath(state.currentBuild->buildId + ".zip");
    const auto extractDir = util::DownloadPath(state.currentBuild->buildId);
    auto dlResult = api_.DownloadFile(state.currentBuild->downloadUrl, zipPath, state.download);
    if (!dlResult.ok) {
      state.buildState = BuildState::Failed;
      state.statusMessage = dlResult.error;
      return;
    }

    if (!api_.VerifyDownloadedFile(zipPath, state.currentBuild->sha256)) {
      state.buildState = BuildState::Failed;
      state.statusMessage = "SHA-256 mismatch from backend manifest.";
      return;
    }

    if (!util::UnzipArchive(zipPath, extractDir)) {
      state.buildState = BuildState::Failed;
      state.statusMessage = "Failed to unzip extension package.";
      return;
    }

    state.buildState = BuildState::Ready;
    state.statusMessage = "Build ready. Use Open folder and load unpacked in chrome://extensions.";
  }

  if (state.currentBuild.has_value()) {
    ImGui::Spacing();
    ImGui::Text("Build ID: %s", state.currentBuild->buildId.c_str());
    ImGui::Text("Issued: %s", state.currentBuild->issuedAt.c_str());
    ImGui::Text("Cooldown until: %s", state.currentBuild->cooldownUntil.c_str());

    const auto total = static_cast<float>(state.download.totalBytes == 0 ? 1 : state.download.totalBytes);
    const auto progress = static_cast<float>(state.download.downloadedBytes) / total;
    ImGui::ProgressBar(progress, ImVec2(-1, 0));
    ImGui::Text("Speed: %.1f KB/s | ETA: %d s", state.download.bytesPerSecond / 1024.0,
                state.download.etaSeconds);

    if (ImGui::Button("Open folder")) {
      util::OpenInFileExplorer(util::DownloadPath(state.currentBuild->buildId));
    }
    ImGui::SameLine();
    if (ImGui::Button("Copy load-unpacked path")) {
      ImGui::SetClipboardText(util::DownloadPath(state.currentBuild->buildId).string().c_str());
    }
  }

  if (!state.statusMessage.empty()) {
    ImGui::Spacing();
    ImGui::TextWrapped("%s", state.statusMessage.c_str());
  }
}

}  // namespace activator
