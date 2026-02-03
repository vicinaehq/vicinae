#pragma once
#include "script-command.hpp"
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

class ScriptCommandGenerator {
public:
  enum class Language : std::uint8_t {
    Bash,
    Python,
    Javascript,
    TypeScript,
    Ruby,
    Perl,
    Php,
    Lua,
    Golang,
    Swift,
  };

  struct LanguageInfo {
    std::string_view templateStr;
    std::string_view name;
  };

  ScriptCommandGenerator() = default;

  static std::optional<Language> parseLanguage(std::string_view lang) {
    static const std::unordered_map<std::string_view, Language> langMap = {
        {"bash", Language::Bash},
        {"python", Language::Python},
        {"javascript", Language::Javascript},
        {"typescript", Language::TypeScript},
        {"ruby", Language::Ruby},
        {"perl", Language::Perl},
        {"php", Language::Php},
        {"lua", Language::Lua},
        {"go", Language::Golang},
        {"swift", Language::Swift},
    };

    auto it = langMap.find(lang);
    if (it == langMap.end()) { return std::nullopt; }
    return it->second;
  }

  static std::vector<std::string_view> supportedLanguages() {
    return {"bash", "python", "javascript", "typescript", "ruby", "perl", "php", "lua", "go", "swift"};
  }

  static std::string generate(std::string_view title, Language lang, script_command::OutputMode mode) {
    // clang-format off
    static const std::unordered_map<Language, LanguageInfo> info = {
      {Language::Bash, {
        "#!/bin/bash\n"
        "# @vicinae.schemaVersion 1\n"
        "# @vicinae.title {title}\n"
        "# @vicinae.mode {mode}\n"
        "# @vicinae.exec [\"/bin/bash\"]\n"
        "\n"
        "echo \"Hello world!\"",
        "Bash"
      }},
      {Language::Python, {
        "#!/usr/bin/env python3\n"
        "# @vicinae.schemaVersion 1\n"
        "# @vicinae.title {title}\n"
        "# @vicinae.mode {mode}\n"
        "# @vicinae.exec [\"/usr/bin/env\", \"python3\"]\n"
        "\n"
        "print(\"Hello world!\")",
        "Python"
      }},
      {Language::Javascript, {
        "#!/usr/bin/env node\n"
        "// @vicinae.schemaVersion 1\n"
        "// @vicinae.title {title}\n"
        "// @vicinae.mode {mode}\n"
        "// @vicinae.exec [\"/usr/bin/env\", \"node\"]\n"
        "\n"
        "console.log('Hello world!');",
        "JavaScript"
      }},
      {Language::TypeScript, {
        "#!/usr/bin/env ts-node\n"
        "// @vicinae.schemaVersion 1\n"
        "// @vicinae.title {title}\n"
        "// @vicinae.mode {mode}\n"
        "// @vicinae.exec [\"/usr/bin/env\", \"ts-node\"]\n"
        "\n"
        "console.log('Hello world!');",
        "TypeScript"
      }},
      {Language::Ruby, {
        "#!/usr/bin/env ruby\n"
        "# @vicinae.schemaVersion 1\n"
        "# @vicinae.title {title}\n"
        "# @vicinae.mode {mode}\n"
        "# @vicinae.exec [\"/usr/bin/env\", \"ruby\"]\n"
        "\n"
        "puts 'Hello world!'",
        "Ruby"
      }},
      {Language::Perl, {
        "#!/usr/bin/env perl\n"
        "# @vicinae.schemaVersion 1\n"
        "# @vicinae.title {title}\n"
        "# @vicinae.mode {mode}\n"
        "# @vicinae.exec [\"/usr/bin/env\", \"perl\"]\n"
        "\n"
        "print \"Hello world!\\n\";",
        "Perl"
      }},
      {Language::Php, {
        "#!/usr/bin/env php\n"
        "<?php\n"
        "// @vicinae.schemaVersion 1\n"
        "// @vicinae.title {title}\n"
        "// @vicinae.mode {mode}\n"
        "// @vicinae.exec [\"/usr/bin/env\", \"php\"]\n"
        "\n"
        "echo \"Hello world!\\n\";",
        "PHP"
      }},
      {Language::Lua, {
        "#!/usr/bin/env lua\n"
        "-- @vicinae.schemaVersion 1\n"
        "-- @vicinae.title {title}\n"
        "-- @vicinae.mode {mode}\n"
        "-- @vicinae.exec [\"/usr/bin/env\", \"lua\"]\n"
        "\n"
        "print(\"Hello world!\")",
        "Lua"
      }},
      {Language::Golang, {
        "#!/usr/bin/env go\n"
        "// @vicinae.schemaVersion 1\n"
        "// @vicinae.title {title}\n"
        "// @vicinae.mode {mode}\n"
        "// @vicinae.exec [\"/usr/bin/env\", \"go\", \"run\"]\n"
        "\n"
        "package main\n"
        "\n"
        "import \"fmt\"\n"
        "\n"
        "func main() {\n"
        "\tfmt.Println(\"Hello world!\")\n"
        "}",
        "Go"
      }},
      {Language::Swift, {
        "#!/usr/bin/env swift\n"
        "// @vicinae.schemaVersion 1\n"
        "// @vicinae.title {title}\n"
        "// @vicinae.mode {mode}\n"
        "// @vicinae.exec [\"/usr/bin/env\", \"swift\"]\n"
        "\n"
        "print(\"Hello world!\")",
        "Swift"
      }},
    };
    // clang-format on

    const auto langInfo = info.find(lang)->second;
    const auto modeStr = script_command::outputModeToString(mode);
    std::string result(langInfo.templateStr);
    size_t pos = 0;

    while ((pos = result.find("{title}", pos)) != std::string::npos) {
      result.replace(pos, 7, title);
      pos += title.length();
    }

    pos = 0;

    while ((pos = result.find("{mode}", pos)) != std::string::npos) {
      result.replace(pos, 6, modeStr);
      pos += modeStr.length();
    }

    return result;
  }
};
