#include <catch2/catch_test_macros.hpp>
#include "script-command.hpp"

using script_command::interpreterForExtension;
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
