#include <catch2/catch_test_macros.hpp>
#include "script-command.hpp"

TEST_CASE("Unknown argument type should fail") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "unknown_type", "placeholder": "test" }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().contains("Unknown argument type"));
}

TEST_CASE("Dropdown without data field should fail") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "dropdown", "placeholder": "select" }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().contains("Dropdown argument must have a 'data' field"));
}

TEST_CASE("Dropdown with data field should succeed") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "dropdown", "placeholder": "select", "data": { "title": "Option", "value": "opt1" } }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->arguments.size() == 1);
  auto &arg = result->arguments.at(0);
  REQUIRE(arg.type == script_command::ArgumentType::Dropdown);
  REQUIRE(arg.placeholder == "select");
  REQUIRE(arg.data.has_value());
  REQUIRE(arg.data->title == "Option");
  REQUIRE(arg.data->value == "opt1");
}

TEST_CASE("Text argument type is correctly parsed") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text", "placeholder": "enter text" }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->arguments.size() == 1);
  REQUIRE(result->arguments.at(0).type == script_command::ArgumentType::Text);
}

TEST_CASE("Password argument type is correctly parsed") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "password", "placeholder": "enter password" }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->arguments.size() == 1);
  REQUIRE(result->arguments.at(0).type == script_command::ArgumentType::Password);
}

TEST_CASE("Optional and percentEncoded defaults work correctly") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text", "placeholder": "test" }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  auto &arg = result->arguments.at(0);
  REQUIRE(arg.optional == false);
  REQUIRE(arg.percentEncoded == false);
}

TEST_CASE("Optional and percentEncoded can be set to true") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text", "placeholder": "test", "optional": true, "percentEncoded": true }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  auto &arg = result->arguments.at(0);
  REQUIRE(arg.optional == true);
  REQUIRE(arg.percentEncoded == true);
}

TEST_CASE("Placeholder should be optional") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text"}
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  auto &arg = result->arguments.at(0);
  REQUIRE(!arg.placeholder.has_value());
}

TEST_CASE("Argument with secure=true field should turn type to password (raycast compat)") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text", "placeholder": "test", "secure": true }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  auto &arg = result->arguments.at(0);
  REQUIRE(arg.type == script_command::ArgumentType::Password);
}

TEST_CASE("Argument with secure=false does nothing (raycast compat)") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "password", "placeholder": "test", "secure": false }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  auto &arg = result->arguments.at(0);
  REQUIRE(arg.type == script_command::ArgumentType::Password);
}
