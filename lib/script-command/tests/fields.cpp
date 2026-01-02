#include <catch2/catch_test_macros.hpp>
#include "script-command.hpp"

TEST_CASE("Optional fields are parsed correctly") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test Command
# @vicinae.mode inline
# @vicinae.author John Doe
# @vicinae.authorURL https://example.com
# @vicinae.description This is a test script
# @vicinae.refreshTime 5m
# @vicinae.currentDirectoryPath /tmp/test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->author.value() == "John Doe");
  REQUIRE(result->authorUrl.value() == "https://example.com");
  REQUIRE(result->description.value() == "This is a test script");
  REQUIRE(result->refreshTime.value() == 300);
  REQUIRE(result->currentDirectoryPath.value() == "/tmp/test");
}

TEST_CASE("Keywords field parses JSON array") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.keywords ["search", "web", "google"]
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->keywords.size() == 3);
  REQUIRE(result->keywords[0] == "search");
  REQUIRE(result->keywords[1] == "web");
  REQUIRE(result->keywords[2] == "google");
}

TEST_CASE("Empty keywords array") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.keywords []
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->keywords.empty());
}

TEST_CASE("Keywords with single item") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.keywords ["search"]
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->keywords.size() == 1);
  REQUIRE(result->keywords[0] == "search");
}

TEST_CASE("Script without keywords has empty vector") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->keywords.empty());
}

TEST_CASE("Invalid keywords JSON should fail") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.keywords [invalid json
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().contains("Failed to parse keywords"));
}

TEST_CASE("Keywords in raycast scope should fail") {
  const char *source = R"(#!/bin/bash
# @raycast.schemaVersion 1
# @raycast.title Test
# @raycast.keywords ["search", "web"]
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().contains("keywords field is only supported in @vicinae scope"));
}

TEST_CASE("Exec field parses JSON array") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.exec ["/bin/bash", "-e"]
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->exec.size() == 2);
  REQUIRE(result->exec[0] == "/bin/bash");
  REQUIRE(result->exec[1] == "-e");
}

TEST_CASE("Exec field is optional") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->exec.empty());
}

TEST_CASE("Exec in raycast scope should fail") {
  const char *source = R"(#!/bin/bash
# @raycast.schemaVersion 1
# @raycast.title Test
# @raycast.exec ["/bin/bash"]
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().contains("exec field is only supported in @vicinae scope"));
}
