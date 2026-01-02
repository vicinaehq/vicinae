#include <catch2/catch_test_macros.hpp>
#include "script-command.hpp"

TEST_CASE("RefreshTime parsing with seconds") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime 30s
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->refreshTime.has_value());
  REQUIRE(result->refreshTime.value() == 30);
}

TEST_CASE("RefreshTime parsing with minutes") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime 5m
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->refreshTime.has_value());
  REQUIRE(result->refreshTime.value() == 300);
}

TEST_CASE("RefreshTime parsing with hours") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime 2h
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->refreshTime.has_value());
  REQUIRE(result->refreshTime.value() == 7200);
}

TEST_CASE("RefreshTime parsing with days") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime 1d
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->refreshTime.has_value());
  REQUIRE(result->refreshTime.value() == 86400);
}

TEST_CASE("RefreshTime parsing without unit defaults to seconds") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime 120
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->refreshTime.has_value());
  REQUIRE(result->refreshTime.value() == 120);
}

TEST_CASE("RefreshTime with whitespace around unit") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime 10 m
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->refreshTime.has_value());
  REQUIRE(result->refreshTime.value() == 600);
}

TEST_CASE("RefreshTime with large values") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime 365d
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->refreshTime.has_value());
  REQUIRE(result->refreshTime.value() == 31536000); // 365 * 24 * 3600
}

TEST_CASE("RefreshTime with invalid unit should fail") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime 5x
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().contains("Unknown time unit"));
}

TEST_CASE("RefreshTime with invalid format should fail") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime abc
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().contains("Invalid time format"));
}

TEST_CASE("RefreshTime with empty value should fail") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().contains("Time string is empty"));
}
