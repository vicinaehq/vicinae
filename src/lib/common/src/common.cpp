#include "common/common.hpp"
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#ifdef __APPLE__
#include <climits>
#include <mach-o/dyld.h>
#endif

#ifdef _WIN32
#include <windows.h>
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
#elif defined(_WIN32)
fs::path selfPath() {
  wchar_t buf[MAX_PATH];
  DWORD const len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
  if (len == 0) return {};
  return fs::path(std::wstring(buf, len));
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

fs::path runtimeDir() {
#ifdef __APPLE__
  if (const char *t = std::getenv("TMPDIR")) return fs::path(t) / "vicinae";
  return "/tmp/vicinae";
#elif defined(_WIN32)
  return fs::temp_directory_path() / "vicinae";
#else
  if (const char *r = std::getenv("XDG_RUNTIME_DIR")) return fs::path(r) / "vicinae";
  return "/tmp/vicinae";
#endif
}

fs::path serverSocketPath() { return runtimeDir() / "vicinae.sock"; }

#ifdef _WIN32
std::string currentUserName() {
  char buf[256];
  DWORD len = sizeof(buf);
  if (GetUserNameA(buf, &len) && len > 1) return std::string(buf, len - 1);
  return "default";
}

std::string serverSocketName() { return "vicinae-" + currentUserName(); }
#else
std::string currentUserName() {
  if (const char *u = std::getenv("USER")) return u;
  if (const char *u = std::getenv("LOGNAME")) return u;
  return "default";
}

std::string serverSocketName() { return serverSocketPath().string(); }
#endif

std::optional<fs::path> findServerBinary() {
#ifdef __APPLE__
  // Inside a .app bundle the server is renamed to "Vicinae" (matches
  // CFBundleExecutable). Flat dev builds keep the Linux name "vicinae-server".
  const auto self = selfPath();
  if (self.parent_path().filename() == "MacOS") { return findHelperProgram("Vicinae"); }
#endif
  return findHelperProgram("vicinae-server");
}

}; // namespace vicinae
