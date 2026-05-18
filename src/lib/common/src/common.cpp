#include "common/common.hpp"
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#ifdef __APPLE__
#include <climits>
#include <mach-o/dyld.h>
#endif

namespace fs = std::filesystem;

namespace vicinae {
#ifdef __APPLE__
fs::path selfPath() {
  char buf[PATH_MAX];
  uint32_t size = sizeof(buf);
  if (_NSGetExecutablePath(buf, &size) == 0) return fs::canonical(buf);

  std::string dyn(size, '\0');
  if (_NSGetExecutablePath(dyn.data(), &size) != 0) return {};
  return fs::canonical(dyn.c_str());
}
#else
fs::path selfPath() { return fs::canonical("/proc/self/exe"); }
#endif

std::vector<fs::path> helperProgramCandidates(std::string_view program) {
  const auto self = selfPath().parent_path();
  std::vector<fs::path> candidates;
  candidates.reserve(3);

  candidates.emplace_back(self / program);
  candidates.emplace_back(self.parent_path() / VICINAE_LIBEXECDIR / program);
  candidates.emplace_back(fs::path{VICINAE_LIBEXEC_PATH} / program);
  return candidates;
}

std::string slurp(std::istream &is) {
  std::ostringstream oss;
  oss << is.rdbuf();
  return oss.str();
}

/**
 * Vicinae relies on a lot of small programs in order to provide various pieces of functionnality
 * such as snippet expansion, clipboard history, or simply launching the server from the CLI.
 * This helper attempts to locate the full pat/h of the binary using only its name, relative to where
 * the caller executable is located.
 */
std::optional<fs::path> findHelperProgram(std::string_view program) {
  std::error_code ec;

  for (const auto &path : helperProgramCandidates(program)) {
    if (fs::is_regular_file(path, ec)) return path;
  }

  return {};
}

std::optional<fs::path> findServerBinary() {
#ifdef __APPLE__
  if (auto p = findHelperProgram("Vicinae")) return p;
#endif
  return findHelperProgram("vicinae-server");
}

}; // namespace vicinae
