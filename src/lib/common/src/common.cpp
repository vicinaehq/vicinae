#include "common/common.hpp"

namespace fs = std::filesystem;

namespace vicinae {
fs::path selfPath() { return fs::read_symlink("/proc/self/exe"); }

/**
 * Vicinae relies on a lot of small programs in order to provide various pieces of functionnality
 * such as snippet expansion, clipboard history, or simply launching the server from the CLI.
 * This helper attempts to locate the full path of the binary using only its name.
 */
std::optional<fs::path> findHelperProgram(std::string_view program) {
  const auto self = selfPath().parent_path();
  fs::path path = self / program;

  if (fs::is_regular_file(path)) return path;
  path = self.parent_path() / "libexec" / "vicinae" / program;
  if (fs::is_regular_file(path)) return path;

  return {};
}

}; // namespace vicinae
