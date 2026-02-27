cmake_minimum_required(VERSION 3.23)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain")
  else()
    message(FATAL_ERROR "VCPKG_ROOT is not set (or invalid). Set it to your vcpkg root before configuring, e.g. PowerShell: $env:VCPKG_ROOT='C:/src/vcpkg' or cmd: set VCPKG_ROOT=C:/src/vcpkg")
  endif()
endif()

project(Activator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(libzip CONFIG REQUIRED)

set(SOURCES
  src/main.cpp
  src/app/App.cpp
  src/ui/Theme.cpp
  src/ui/Screens/LoginScreen.cpp
  src/ui/Screens/DashboardScreen.cpp
  src/ui/Screens/DownloadScreen.cpp
  src/ui/Screens/SettingsScreen.cpp
  src/ui/Widgets/Toasts.cpp
  src/ui/Widgets/Spinner.cpp
  src/net/Http.cpp
  src/net/ApiClient.cpp
  src/security/Hwid.cpp
  src/security/Crypto.cpp
  src/security/SecureStore.cpp
  src/security/RateLimit.cpp
  src/util/Files.cpp
  src/util/Time.cpp
  src/util/Strings.cpp
  src/util/Zip.cpp
)

add_executable(Activator ${SOURCES})

target_include_directories(Activator PRIVATE src)

target_link_libraries(Activator PRIVATE
  glfw
  OpenGL::GL
  imgui::imgui
  cpr::cpr
  OpenSSL::SSL
  OpenSSL::Crypto
  nlohmann_json::nlohmann_json
  fmt::fmt
  libzip::zip
)

if(WIN32)
  target_link_libraries(Activator PRIVATE bcrypt crypt32 advapi32)
endif()

set_target_properties(Activator PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
