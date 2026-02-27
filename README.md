# Activator (C++20 + Dear ImGui)

Windows-first desktop activator built with Dear ImGui, GLFW, and OpenGL3.

## What this app does

- Login with a **license key** (via an adapter over your existing KeyAuth integration).
- Locally derive a stable **HWID hash** (MachineGuid + CPU id + C: volume serial, SHA-256).
- Verify entitlement with backend and show binding status:
  - Bound to this PC
  - Bound to another PC (with cooldown)
- Request a **custom extension build** bound server-side to license + hwidHash.
- Download ZIP at runtime, verify SHA-256, unzip into `%APPDATA%\Activator\downloads\<buildId>`.
- Show manual Chrome MV3 load-unpacked instructions.
- Store session secret encrypted at rest (Windows DPAPI).

## Assumptions

1. Backend exists and exposes:
   - `POST /v1/entitlements/verify`
   - `POST /v1/build/request`
   - `GET /v1/build/status?buildId=...`
2. KeyAuth is source of truth for license/subscription state.
3. Extension ZIP is delivered at runtime; no ZIP/binary assets are committed.
4. Server is authoritative for cooldown and HWID binding policy.

## Security model notes

- **Never stores license plaintext**. UI keeps it in-memory only for login; masked value is shown.
- Session token is persisted as DPAPI-encrypted blob in `%APPDATA%\Activator\session_token.bin`.
- Download package integrity is checked by SHA-256 against backend response before unzip.
- TLS certificate validation is enforced through cpr/curl defaults (`VerifySsl=true`).
- Logs must never include license, tokens, or full auth URLs.

## Better policy implemented

Client UX is designed for server policy:
- 1 active HWID binding at a time per license.
- Rebind cooldown (typically 24h), overridable server-side.
- Client displays cooldown/binding states but does not enforce security.

## Build requirements

- Windows 10/11
- MSVC (Visual Studio 2022 recommended)
- CMake >= 3.23
- Ninja
- vcpkg

## Configure + build

```powershell
$env:VCPKG_ROOT="C:\src\vcpkg"
cmake --preset debug
cmake --build --preset debug
```

Release:

```powershell
cmake --preset release
cmake --build --preset release
```

Output executable:

- `build/debug/bin/Activator.exe`
- `build/release/bin/Activator.exe`

## Runtime configuration

Set backend URL through env var:

```powershell
$env:ACTIVATOR_BACKEND_URL="https://api.example.com"
```

Or update in settings UI/state defaults.

## Chrome manual load-unpacked flow

1. Request + download build.
2. Click **Open folder**.
3. Open `chrome://extensions`.
4. Enable **Developer mode**.
5. Click **Load unpacked** and select extracted build folder.

## Repo structure

```text
/
  CMakeLists.txt
  CMakePresets.json
  vcpkg.json
  README.md
  src/
    main.cpp
    app/
      App.cpp App.hpp
      State.hpp
    ui/
      Theme.cpp Theme.hpp
      Screens/
        LoginScreen.cpp LoginScreen.hpp
        DashboardScreen.cpp DashboardScreen.hpp
        DownloadScreen.cpp DownloadScreen.hpp
        SettingsScreen.cpp SettingsScreen.hpp
      Widgets/
        Toasts.cpp Toasts.hpp
        Spinner.cpp Spinner.hpp
    net/
      ApiClient.cpp ApiClient.hpp
      Http.cpp Http.hpp
    security/
      Hwid.cpp Hwid.hpp
      Crypto.cpp Crypto.hpp
      SecureStore.cpp SecureStore.hpp
      RateLimit.cpp RateLimit.hpp
    util/
      Files.cpp Files.hpp
      Time.cpp Time.hpp
      Strings.cpp Strings.hpp
      Zip.cpp Zip.hpp
  assets/
```

## KeyAuth integration adapter

`ApiClient::ValidateKeyAuthLicense(...)` is intentionally an adapter stub entry point.
Replace its body with your existing KeyAuth integration call and map fields:
- token
- userId
- username
- plan
- expiresAt

No other code should need changes.
