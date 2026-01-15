#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include "script-command.hpp"

namespace fs = std::filesystem;

static const fs::path tmpDir = "/tmp";

TEST_CASE("Parse in-memory script with arguments") {
  const char *source = R"(#!/bin/bash -c hello

# Required parameters:
# @vicinae.schemaVersion 1
# @vicinae.title Search Flights
# @vicinae.mode silent

# Optional parameters:
# @vicinae.icon 🛩
# @vicinae.packageName Web Searches
# @vicinae.argument1 { "type": "text", "placeholder": "from city", "percentEncoded": true }
# @vicinae.argument2 { "type": "text", "placeholder": "to city", "optional": true, "percentEncoded": true }

open "https://www.google.com/search?q=flights%20from%20$1%20to%20$2"
)";

  auto result = script_command::ScriptCommand::parse(source);

  REQUIRE(result.has_value());
  REQUIRE(result->title == "Search Flights");
  REQUIRE(result->schemaVersion == "1");
  REQUIRE(result->mode == script_command::OutputMode::Silent);
  REQUIRE(result->packageName == "Web Searches");
  REQUIRE(result->icon.value() == "🛩");

  auto &args = result->arguments;

  REQUIRE(args.size() == 2);

  auto &arg1 = args.at(0);

  REQUIRE(arg1.type == script_command::ArgumentType::Text);
  REQUIRE(arg1.placeholder == "from city");
  REQUIRE(arg1.percentEncoded);

  auto &arg2 = args.at(1);

  REQUIRE(arg2.type == script_command::ArgumentType::Text);
  REQUIRE(arg2.placeholder == "to city");
  REQUIRE(arg2.optional);
  REQUIRE(arg2.percentEncoded);
};

TEST_CASE("Parse file script with arguments") {
  const char *source = R"(#!/bin/bash -c hello

# Required parameters:
# @vicinae.schemaVersion 1
# @vicinae.title Search Flights
# @vicinae.mode silent

# Optional parameters:
# @vicinae.icon 🛩
# @vicinae.packageName Web Searches
# @vicinae.argument1 { "type": "text", "placeholder": "from city", "percentEncoded": true }
# @vicinae.argument2 { "type": "text", "placeholder": "to city", "optional": true, "percentEncoded": true }

open "https://www.google.com/search?q=flights%20from%20$1%20to%20$2"
)";

  std::ofstream(tmpDir / "script.txt") << source;
  std::ifstream ifs(tmpDir / "script.txt");
  const auto result = script_command::ScriptCommand::parse(ifs);

  REQUIRE(result.has_value());
  REQUIRE(result->title == "Search Flights");
  REQUIRE(result->schemaVersion == "1");
  REQUIRE(result->mode == script_command::OutputMode::Silent);
  REQUIRE(result->packageName == "Web Searches");
  REQUIRE(result->icon.value() == "🛩");

  auto &args = result->arguments;

  REQUIRE(args.size() == 2);

  auto &arg1 = args.at(0);

  REQUIRE(arg1.type == script_command::ArgumentType::Text);
  REQUIRE(arg1.placeholder == "from city");
  REQUIRE(arg1.percentEncoded);

  auto &arg2 = args.at(1);

  REQUIRE(arg2.type == script_command::ArgumentType::Text);
  REQUIRE(arg2.placeholder == "to city");
  REQUIRE(arg2.optional);
  REQUIRE(arg2.percentEncoded);
};
