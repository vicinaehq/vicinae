#include "common/common.hpp"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace vicinae {
fs::path selfPath() { return fs::canonical("/proc/self/exe"); }

std::vector<fs::path> helperProgramCandidates(std::string_view program) {
  const auto self = selfPath().parent_path();
  return {
      self / program,
      self.parent_path() / VICINAE_LIBEXECDIR / program,
      fs::path{VICINAE_LIBEXEC_PATH} / program,
  };
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
