#include <catch2/catch_test_macros.hpp>
#include "script-command.hpp"

TEST_CASE("Hash (#) comments are parsed correctly") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Hash Comment Test
# @vicinae.mode compact
echo "Hello"
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Hash Comment Test");
  REQUIRE(result->mode == script_command::OutputMode::Compact);
}

TEST_CASE("Double slash (//) comments are parsed correctly") {
  const char *source = R"(#!/usr/bin/env node
// @vicinae.schemaVersion 1
// @vicinae.title JS Comment Test
// @vicinae.mode compact
console.log("Hello");
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "JS Comment Test");
  REQUIRE(result->mode == script_command::OutputMode::Compact);
}

TEST_CASE("Double dash (--) comments are parsed correctly") {
  const char *source = R"(#!/usr/bin/env lua
-- @vicinae.schemaVersion 1
-- @vicinae.title Lua Comment Test
-- @vicinae.mode compact
print("Hello")
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Lua Comment Test");
  REQUIRE(result->mode == script_command::OutputMode::Compact);
}

TEST_CASE("Lua script with all metadata fields using -- comments") {
  const char *source = R"(#!/usr/bin/env lua
-- @vicinae.schemaVersion 1
-- @vicinae.title Full Lua Test
-- @vicinae.mode inline
-- @vicinae.refreshTime 30s
-- @vicinae.author Test Author
-- @vicinae.description A test Lua script
print("Hello World")
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Full Lua Test");
  REQUIRE(result->mode == script_command::OutputMode::Inline);
  REQUIRE(result->refreshTime.value() == 30);
  REQUIRE(result->author.value() == "Test Author");
  REQUIRE(result->description.value() == "A test Lua script");
}

TEST_CASE("Mixed comment styles should work (different lines)") {
  const char *source = R"(#!/usr/bin/env lua
-- @vicinae.schemaVersion 1
-- @vicinae.title Mixed Test
# Some other comment (not a metadata comment)
print("Hello")
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Mixed Test");
}

TEST_CASE("Double dash should not conflict with command arguments") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Arg Test
echo "test" -- this is a regular comment in the code
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Arg Test");
}

TEST_CASE("Comments with leading whitespace and -- marker") {
  const char *source = R"(#!/usr/bin/env lua
  -- @vicinae.schemaVersion 1
  -- @vicinae.title Whitespace Test
  -- @vicinae.mode compact
print("Hello")
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Whitespace Test");
  REQUIRE(result->mode == script_command::OutputMode::Compact);
}

TEST_CASE("TypeScript with // comments") {
  const char *source = R"(#!/usr/bin/env ts-node
// @vicinae.schemaVersion 1
// @vicinae.title TypeScript Test
// @vicinae.mode terminal
// @vicinae.keywords ["typescript", "test"]
console.log("Hello from TypeScript");
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "TypeScript Test");
  REQUIRE(result->mode == script_command::OutputMode::Terminal);
  REQUIRE(result->keywords.size() == 2);
  REQUIRE(result->keywords[0] == "typescript");
  REQUIRE(result->keywords[1] == "test");
}

TEST_CASE("Go with // comments") {
  const char *source = R"(#!/usr/bin/env go run
// @vicinae.schemaVersion 1
// @vicinae.title Go Test
// @vicinae.mode compact
package main
import "fmt"
func main() { fmt.Println("Hello") }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Go Test");
  REQUIRE(result->mode == script_command::OutputMode::Compact);
}

TEST_CASE("PHP with // comments after <?php tag") {
  const char *source = R"(#!/usr/bin/env php
<?php
// @vicinae.schemaVersion 1
// @vicinae.title PHP Test
// @vicinae.mode compact
echo "Hello from PHP\n";
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "PHP Test");
  REQUIRE(result->mode == script_command::OutputMode::Compact);
}

TEST_CASE("Semicolon (;) comments are parsed correctly") {
  const char *source = R"(#!/usr/bin/env sbcl --script
; @vicinae.schemaVersion 1
; @vicinae.title Semicolon Comment Test
; @vicinae.mode compact
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Semicolon Comment Test");
  REQUIRE(result->mode == script_command::OutputMode::Compact);
}
