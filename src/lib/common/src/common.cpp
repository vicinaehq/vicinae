#include "common/common.hpp"
#include <cctype>
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
  candidates.reserve(4);

  candidates.emplace_back(self / program);

#ifdef __APPLE__
  // Inside Vicinae.app/Contents/MacOS the bundle's main executable is "Vicinae"
  // (capitalised target output name), not "vicinae-server". When the CLI asks
  // for "vicinae-server" we also try a capitalised first letter so it finds its
  // sibling.
  if (!program.empty()) {
    std::string capitalised;
    capitalised.reserve(program.size());
    capitalised.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(program.front()))));
    capitalised.append(program.substr(1));
    // strip trailing "-server" so "vicinae-server" -> "Vicinae"
    if (auto const dash = capitalised.find('-'); dash != std::string::npos) {
      capitalised.resize(dash);
    }
    if (capitalised != program) { candidates.emplace_back(self / capitalised); }
  }
#endif

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

}; // namespace vicinae
