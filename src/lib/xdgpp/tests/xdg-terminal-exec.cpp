#include "utils/utils.hpp"
#include "xdg-terminal-exec/xdg-terminals-list.hpp"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

TEST_CASE("Should write the new terminal app ID to an empty file") {
  const auto path = std::filesystem::temp_directory_path() / "xdg-terminals.list";
  std::filesystem::remove(path);

  REQUIRE(xdgpp::setDefaultTerminal("test", {}, path));
  REQUIRE(slurp(path) == "# Configured by the Vicinae launcher\ntest\n");
}

TEST_CASE("Should write the new terminal app ID to an existing file, before all other entries") {
  const auto path = std::filesystem::temp_directory_path() / "xdg-terminals.list";
  std::filesystem::remove(path);

  {
    std::ofstream ofs{path};
    ofs << "org.someone.something\n";
  }

  REQUIRE(xdgpp::setDefaultTerminal("test", {}, path));
  REQUIRE(slurp(path) == "# Configured by the Vicinae launcher\ntest\norg.someone.something\n");
}

TEST_CASE("Should write the new terminal app ID to an already modified file") {
  const auto path = std::filesystem::temp_directory_path() / "xdg-terminals.list";
  std::filesystem::remove(path);

  {
    std::ofstream ofs{path};
    ofs << "# Configured by the Vicinae launcher\n# This is some "
           "comment\norg.someone.something\norg.somethingelse.unrelated\n";
  }

  REQUIRE(xdgpp::setDefaultTerminal("test", {}, path));
  REQUIRE(
      slurp(path) ==
      "# Configured by the Vicinae launcher\n# This is some comment\ntest\norg.somethingelse.unrelated\n");
}
