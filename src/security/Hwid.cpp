#include "security/Hwid.hpp"

#include <sstream>

#include "security/Crypto.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <intrin.h>
#endif

namespace activator::security {

namespace {
#ifdef _WIN32
std::string ReadMachineGuid() {
  HKEY key;
  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0,
                    KEY_READ | KEY_WOW64_64KEY, &key) != ERROR_SUCCESS) {
    return "noguid";
  }

  char buffer[128]{};
  DWORD size = sizeof(buffer);
  auto result = RegQueryValueExA(key, "MachineGuid", nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(buffer), &size);
  RegCloseKey(key);
  return result == ERROR_SUCCESS ? std::string(buffer) : "noguid";
}

std::string CpuIdString() {
  int data[4]{};
  __cpuid(data, 0);
  std::ostringstream oss;
  oss << std::hex << data[0] << '-' << data[1] << '-' << data[2] << '-' << data[3];
  return oss.str();
}

std::string SystemVolumeSerial() {
  DWORD serial = 0;
  if (!GetVolumeInformationA("C:\\", nullptr, 0, &serial, nullptr, nullptr, nullptr, 0)) {
    return "novol";
  }
  std::ostringstream oss;
  oss << std::hex << serial;
  return oss.str();
}
#endif
}  // namespace

std::string GetHwidHash() {
#ifdef _WIN32
  const std::string source = ReadMachineGuid() + "|" + CpuIdString() + "|" + SystemVolumeSerial();
#else
  const std::string source = "unsupported-platform";
#endif
  return Sha256Hex(source);
}

}  // namespace activator::security
