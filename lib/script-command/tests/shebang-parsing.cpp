#include <catch2/catch_test_macros.hpp>
#include "script-command.hpp"

TEST_CASE("Parse simple shebang") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->exec.size() == 1);
  REQUIRE(result->exec[0] == "/bin/bash");
}

TEST_CASE("Parse shebang with arguments") {
  const char *source = R"(#!/bin/bash -c hello
# @vicinae.schemaVersion 1
# @vicinae.title Test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->exec.size() == 3);
  REQUIRE(result->exec[0] == "/bin/bash");
  REQUIRE(result->exec[1] == "-c");
  REQUIRE(result->exec[2] == "hello");
}

TEST_CASE("Parse shebang with env") {
  const char *source = R"(#!/usr/bin/env python3
# @vicinae.schemaVersion 1
# @vicinae.title Test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->exec.size() == 2);
  REQUIRE(result->exec[0] == "/usr/bin/env");
  REQUIRE(result->exec[1] == "python3");
}

TEST_CASE("Parse shebang with multiple arguments") {
  const char *source = R"(#!/usr/bin/node --experimental-modules --no-warnings
# @vicinae.schemaVersion 1
# @vicinae.title Test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->exec.size() == 3);
  REQUIRE(result->exec[0] == "/usr/bin/node");
  REQUIRE(result->exec[1] == "--experimental-modules");
  REQUIRE(result->exec[2] == "--no-warnings");
}

TEST_CASE("Script without shebang should have empty command line") {
  const char *source = R"(# @vicinae.schemaVersion 1
# @vicinae.title Test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->exec.empty());
}

TEST_CASE("Shebang with extra whitespace") {
  const char *source = R"(#!/bin/bash   -c   hello
# @vicinae.schemaVersion 1
# @vicinae.title Test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->exec.size() == 3);
  REQUIRE(result->exec[0] == "/bin/bash");
  REQUIRE(result->exec[1] == "-c");
  REQUIRE(result->exec[2] == "hello");
}

TEST_CASE("Empty shebang (just #!) should be zero arguments") {
  const char *source = R"(#!
# @vicinae.schemaVersion 1
# @vicinae.title Test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->exec.empty());
}
