#include "util/Zip.hpp"

#include <zip.h>

#include <cstring>
#include <fstream>

namespace activator::util {

bool UnzipArchive(const std::filesystem::path& archivePath, const std::filesystem::path& destination) {
  int error = 0;
  zip_t* archive = zip_open(archivePath.string().c_str(), ZIP_RDONLY, &error);
  if (archive == nullptr) {
    return false;
  }

  std::filesystem::create_directories(destination);

  const zip_int64_t count = zip_get_num_entries(archive, 0);
  for (zip_uint64_t i = 0; i < static_cast<zip_uint64_t>(count); ++i) {
    struct zip_stat st {};
    if (zip_stat_index(archive, i, 0, &st) != 0) {
      zip_close(archive);
      return false;
    }

    const std::filesystem::path outPath = destination / st.name;
    if (st.name[std::strlen(st.name) - 1] == '/') {
      std::filesystem::create_directories(outPath);
      continue;
    }

    std::filesystem::create_directories(outPath.parent_path());
    zip_file_t* file = zip_fopen_index(archive, i, 0);
    if (file == nullptr) {
      zip_close(archive);
      return false;
    }

    std::ofstream out(outPath, std::ios::binary);
    char buffer[4096];
    zip_int64_t bytesRead = 0;
    while ((bytesRead = zip_fread(file, buffer, sizeof(buffer))) > 0) {
      out.write(buffer, bytesRead);
    }
    zip_fclose(file);
  }

  zip_close(archive);
  return true;
}

}  // namespace activator::util
