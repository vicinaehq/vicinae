#include <catch2/catch_test_macros.hpp>
#include "script-command.hpp"

TEST_CASE("Empty file should fail") {
  const char *source = "";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
}

TEST_CASE("File with only shebang should fail") {
  const char *source = "#!/bin/bash\n";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
}

TEST_CASE("Missing title should fail") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.mode silent
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
}

TEST_CASE("Malformed JSON in argument should fail") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { invalid json }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
}

TEST_CASE("Incomplete JSON in argument should fail") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text"
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
}

TEST_CASE("Parse script with // comment style") {
  const char *source = R"(#!/bin/bash
// @vicinae.schemaVersion 1
// @vicinae.title Double Slash Test
// @vicinae.mode compact
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Double Slash Test");
  REQUIRE(result->mode == script_command::OutputMode::Compact);
}

TEST_CASE("Parse script with mixed whitespace") {
  const char *source = "#!/bin/bash\n"
                       "#   @vicinae.schemaVersion   1  \n"
                       "#\t@vicinae.title\t  Whitespace Test  \n"
                       "#  @vicinae.mode   silent  \n";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Whitespace Test");
  REQUIRE(result->mode == script_command::OutputMode::Silent);
}

TEST_CASE("needsConfirmation with true value") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.needsConfirmation true
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->needsConfirmation == true);
}

TEST_CASE("needsConfirmation with false value") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.needsConfirmation false
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->needsConfirmation == false);
}

TEST_CASE("All output modes") {
  SECTION("fullOutput mode") {
    const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode fullOutput
)";
    auto result = script_command::ScriptCommand::parse(source);
    REQUIRE(result.has_value());
    REQUIRE(result->mode == script_command::OutputMode::Full);
  }

  SECTION("compact mode") {
    const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode compact
)";
    auto result = script_command::ScriptCommand::parse(source);
    REQUIRE(result.has_value());
    REQUIRE(result->mode == script_command::OutputMode::Compact);
  }

  SECTION("inline mode") {
    const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
# @vicinae.refreshTime 1d
)";
    auto result = script_command::ScriptCommand::parse(source);
    REQUIRE(result.has_value());
    REQUIRE(result->mode == script_command::OutputMode::Inline);
  }

  SECTION("silent mode") {
    const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode silent
)";
    auto result = script_command::ScriptCommand::parse(source);
    REQUIRE(result.has_value());
    REQUIRE(result->mode == script_command::OutputMode::Silent);
  }

  SECTION("terminal mode") {
    const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode terminal
)";
    auto result = script_command::ScriptCommand::parse(source);
    REQUIRE(result.has_value());
    REQUIRE(result->mode == script_command::OutputMode::Terminal);
  }
}

TEST_CASE("Argument with percentEncoded true") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text", "placeholder": "query", "percentEncoded": true }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->arguments.size() == 1);
  auto &arg = result->arguments.at(0);
  REQUIRE(arg.type == script_command::ArgumentType::Text);
  REQUIRE(arg.placeholder == "query");
  REQUIRE(arg.percentEncoded == true);
}

TEST_CASE("Argument with percentEncoded false") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text", "placeholder": "query", "percentEncoded": false }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->arguments.size() == 1);
  auto &arg = result->arguments.at(0);
  REQUIRE(arg.percentEncoded == false);
}

TEST_CASE("Argument without percentEncoded field defaults to false") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text", "placeholder": "query" }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->arguments.size() == 1);
  auto &arg = result->arguments.at(0);
  REQUIRE(arg.percentEncoded == false);
}

TEST_CASE("Argument with optional true") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text", "placeholder": "query", "optional": true }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->arguments.size() == 1);
  auto &arg = result->arguments.at(0);
  REQUIRE(arg.optional == true);
}

TEST_CASE("Argument with optional false") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text", "placeholder": "query", "optional": false }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->arguments.size() == 1);
  auto &arg = result->arguments.at(0);
  REQUIRE(arg.optional == false);
}

TEST_CASE("Multiple arguments in order") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.argument1 { "type": "text", "placeholder": "first" }
# @vicinae.argument2 { "type": "password", "placeholder": "second" }
# @vicinae.argument3 { "type": "dropdown", "placeholder": "third", "data": { "title": "Option", "value": "val" } }
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->arguments.size() == 3);
  REQUIRE(result->arguments.at(0).placeholder == "first");
  REQUIRE(result->arguments.at(1).placeholder == "second");
  REQUIRE(result->arguments.at(2).placeholder == "third");
  REQUIRE(result->arguments.at(1).type == script_command::ArgumentType::Password);
  REQUIRE(result->arguments.at(2).type == script_command::ArgumentType::Dropdown);
}

TEST_CASE("Script keeps parsing after non comment") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
echo "This is code"
# @vicinae.mode silent
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->schemaVersion == "1");
  REQUIRE(result->title == "Test");
  REQUIRE(result->mode == script_command::OutputMode::Silent);
}

TEST_CASE("Unknown scope should be ignored") {
  const char *source = R"(#!/bin/bash
# @unknown.schemaVersion 1
# @vicinae.schemaVersion 1
# @vicinae.title Test
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Test");
}

TEST_CASE("Title with special characters") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test & Special <Characters> "Quotes"
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Test & Special <Characters> \"Quotes\"");
}

TEST_CASE("Very long title") {
  std::string longTitle(1000, 'a');
  std::string source = "#!/bin/bash\n"
                       "# @vicinae.schemaVersion 1\n"
                       "# @vicinae.title " +
                       longTitle + "\n";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == longTitle);
}

TEST_CASE("Raycast scope works") {
  const char *source = R"(#!/bin/bash
# @raycast.schemaVersion 1
# @raycast.title Raycast Test
# @raycast.mode silent
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Raycast Test");
}

TEST_CASE("Empty lines and whitespace-only lines are ignored") {
  const char *source = "#!/bin/bash\n"
                       "\n"
                       "#   \n"
                       "# @vicinae.schemaVersion 1\n"
                       "   \n"
                       "# @vicinae.title Test\n"
                       "#\n";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->title == "Test");
}

TEST_CASE("inline output mode without refreshTime is valid") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode inline
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(result.has_value());
  REQUIRE(result->mode == script_command::OutputMode::Inline);
  REQUIRE(!result->refreshTime.has_value());
}

TEST_CASE("needs inline output mode when refreshTime is present") {
  const char *source = R"(#!/bin/bash
# @vicinae.schemaVersion 1
# @vicinae.title Test
# @vicinae.mode silent
# @vicinae.refreshTime 1d
)";
  auto result = script_command::ScriptCommand::parse(source);
  REQUIRE(!result.has_value());
}
