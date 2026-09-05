#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include "script-command.hpp"

using script_command::interpreterForExtension;
using script_command::resolveInterpreter;
using script_command::ScriptCommand;
using script_command::shebangInterpreter;
using Argv = std::vector<std::string>;

TEST_CASE("Shebang line is recorded") {
  const char *source = R"(#!/usr/bin/env python3
# @vicinae.schemaVersion 1
# @vicinae.title Shebang Test
)";
  auto result = ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->shebang == Argv{"/usr/bin/env", "python3"});
}

TEST_CASE("Shebang with CRLF line ending is trimmed") {
  const char *source = "#!/bin/bash -x\r\n# @vicinae.schemaVersion 1\r\n# @vicinae.title CRLF\r\n";
  auto result = ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->shebang == Argv{"/bin/bash", "-x"});
}

TEST_CASE("Missing shebang leaves the field empty") {
  const char *source = R"(# @vicinae.schemaVersion 1
# @vicinae.title No Shebang
)";
  auto result = ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->shebang.empty());
}

TEST_CASE("Interpreter is derived from the shebang") {
  REQUIRE(shebangInterpreter(Argv{"/usr/bin/env", "python3"}) == Argv{"python3"});
  REQUIRE(shebangInterpreter(Argv{"/usr/bin/env", "-S", "bash", "-x"}) == Argv{"bash", "-x"});
  REQUIRE(shebangInterpreter(Argv{"/bin/bash", "-x"}) == Argv{"bash", "-x"});
  REQUIRE(shebangInterpreter(Argv{"/usr/bin/env"}).empty());
  REQUIRE(shebangInterpreter(Argv{}).empty());
}

TEST_CASE("Interpreter is derived from the extension") {
  REQUIRE(interpreterForExtension(".py") == Argv{"python"});
  REQUIRE(interpreterForExtension(".ps1").front() == "pwsh");
  REQUIRE(interpreterForExtension(".bat").empty());
  REQUIRE(interpreterForExtension("").empty());
}

TEST_CASE("Interpreter resolution goes through the lookup with alias fallback") {
  const auto lookup = [](std::string_view name) -> std::optional<std::string> {
    if (name == "python") return "C:/python/python.exe";
    if (name == "powershell") return "C:/win/powershell.exe";
    return std::nullopt;
  };

  ScriptCommand withShebang;
  withShebang.shebang = {"/usr/bin/env", "python3"};
  auto resolved = resolveInterpreter(withShebang, "script", lookup);
  REQUIRE(resolved.has_value());
  REQUIRE(*resolved == Argv{"C:/python/python.exe"});

  ScriptCommand byExtension;
  resolved = resolveInterpreter(byExtension, "script.ps1", lookup);
  REQUIRE(resolved.has_value());
  REQUIRE(resolved->front() == "C:/win/powershell.exe");
  REQUIRE(resolved->back() == "-File");

  ScriptCommand missing;
  missing.shebang = {"/usr/bin/ruby"};
  REQUIRE(!resolveInterpreter(missing, "script", lookup).has_value());

  ScriptCommand direct;
  resolved = resolveInterpreter(direct, "script.bat", lookup);
  REQUIRE(resolved.has_value());
  REQUIRE(resolved->empty());
}

TEST_CASE("An existing shebang program is used verbatim") {
  const auto interpreter = std::filesystem::temp_directory_path() / "vicinae-shebang-test-interpreter";
  { std::ofstream(interpreter) << ""; }
  const auto lookup = [](std::string_view) -> std::optional<std::string> { return std::nullopt; };

  ScriptCommand cmd;
  cmd.shebang = {interpreter.string(), "-S", "flag"};
  auto resolved = resolveInterpreter(cmd, "script", lookup);
  std::filesystem::remove(interpreter);

  REQUIRE(resolved.has_value());
  REQUIRE(*resolved == Argv{interpreter.string(), "-S", "flag"});
}
