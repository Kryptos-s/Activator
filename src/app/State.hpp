#pragma once

#include <chrono>
#include <deque>
#include <optional>
#include <string>
#include <vector>

namespace activator {

enum class View { Login, Dashboard, Downloads, Settings };
enum class AuthState { LoggedOut, LoggingIn, LoggedIn, Error };
enum class BuildState { Idle, Queued, Building, Ready, Downloading, Cooldown, Failed };

enum class ToastType { Info, Success, Error };

struct ToastMessage {
  ToastType type{ToastType::Info};
  std::string text;
  std::chrono::steady_clock::time_point expiresAt;
};

struct DownloadProgress {
  std::size_t downloadedBytes{0};
  std::size_t totalBytes{0};
  double bytesPerSecond{0.0};
  int etaSeconds{-1};
};

struct Identity {
  std::string userId;
  std::string username;
  std::string plan;
  std::string expiresAt;
};

struct BuildInfo {
  std::string buildId;
  std::string downloadUrl;
  std::string sha256;
  std::string cooldownUntil;
  std::string expiresAt;
  std::string issuedAt;
};

struct AppState {
  View view{View::Login};
  AuthState authState{AuthState::LoggedOut};
  BuildState buildState{BuildState::Idle};
  std::string backendUrl;
  std::string authToken;
  std::string maskedLicense;
  std::string hwidHash;
  std::string hwidShort;
  std::string statusMessage;
  std::string diagnostics;
  bool debugLogging{false};
  bool boundToThisPc{true};
  std::optional<Identity> identity;
  std::optional<BuildInfo> currentBuild;
  DownloadProgress download;
  std::deque<std::string> recentLogs;
  std::vector<ToastMessage> toasts;
};

}  // namespace activator
