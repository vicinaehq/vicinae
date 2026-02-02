#pragma once
#include <cstdint>
#include <expected>
#include <filesystem>
#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

namespace script_command {

enum class OutputMode : std::uint8_t { Full, Compact, Inline, Silent, Terminal };
enum class ArgumentType : std::uint8_t { Text, Password, Dropdown };

std::string_view outputModeToString(OutputMode mode);
std::optional<OutputMode> parseOutputMode(std::string_view str);

struct ArgumentDataOption {
  std::string title;
  std::string value;
};

struct ParsedScriptArgument {
  std::string type;
  std::optional<std::string> placeholder;
  std::optional<bool> optional;
  std::optional<bool> percentEncoded;
  std::optional<ArgumentDataOption> data;
  std::optional<bool> secure; // legacy, for raycast compat
};

struct ScriptArgument {
  ArgumentType type;
  std::optional<std::string> placeholder;
  bool optional = false;
  bool percentEncoded = false;
  std::optional<ArgumentDataOption> data;
};

struct ScriptCommand {
  std::vector<std::string> exec;
  std::string schemaVersion;
  std::string title;
  OutputMode mode;
  std::optional<std::string> packageName;
  std::optional<std::string> icon;
  std::optional<std::string> iconDark;
  std::optional<std::filesystem::path> currentDirectoryPath;
  bool needsConfirmation = false;
  std::optional<std::uint64_t> refreshTime; // in seconds
  std::optional<std::string> author;
  std::optional<std::string> authorUrl;
  std::optional<std::string> description;
  std::vector<std::string> keywords;
  std::vector<ScriptArgument> arguments;

  static std::expected<ScriptCommand, std::string> parse(std::string_view str);
  static std::expected<ScriptCommand, std::string> parse(std::ifstream &ifs);
  static std::expected<ScriptCommand, std::string> fromFile(const std::filesystem::path &path);
};

std::ostream &operator<<(std::ostream &ofs, const ScriptCommand &cmd);

} // namespace script_command
