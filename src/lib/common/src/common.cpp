#include "common/common.hpp"

namespace fs = std::filesystem;

namespace vicinae {
fs::path selfPath() { return fs::canonical("/proc/self/exe"); }

/**
 * Vicinae relies on a lot of small programs in order to provide various pieces of functionnality
 * such as snippet expansion, clipboard history, or simply launching the server from the CLI.
 * This helper attempts to locate the full pat/h of the binary using only its name, relative to where
 * the caller executable is located.
 */
std::optional<fs::path> findHelperProgram(std::string_view program) {
  const auto self = selfPath().parent_path();
  fs::path path = self / program;

  if (fs::is_regular_file(path)) return path;
  path = self.parent_path() / VICINAE_LIBEXECDIR / program;
  if (fs::is_regular_file(path)) return path;

  return {};
}

}; // namespace vicinae
