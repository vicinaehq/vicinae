#include "script-command.hpp"
#include <algorithm>
#include <charconv>
#include <format>
#include <fstream>
#include <glaze/json/read.hpp>
#include <iostream>
#include <ranges>

namespace script_command {

std::string_view outputModeToString(OutputMode mode) {
  switch (mode) {
  case OutputMode::Full:
    return "fullOutput";
  case OutputMode::Compact:
    return "compact";
  case OutputMode::Inline:
    return "inline";
  case OutputMode::Silent:
    return "silent";
  case OutputMode::Terminal:
    return "terminal";
  default:
    return "fullOutput";
  }
}

std::optional<OutputMode> parseOutputMode(std::string_view str) {
  if (str == "fullOutput") {
    return OutputMode::Full;
  } else if (str == "compact") {
    return OutputMode::Compact;
  } else if (str == "inline") {
    return OutputMode::Inline;
  } else if (str == "silent") {
    return OutputMode::Silent;
  } else if (str == "terminal") {
    return OutputMode::Terminal;
  }
  return std::nullopt;
}

std::ostream &operator<<(std::ostream &ofs, const ScriptCommand &cmd) {
  std::println(ofs, "schemaVersion => {}", cmd.schemaVersion);
  std::println(ofs, "title => {}", cmd.title);
  std::println(ofs, "mode => {}", static_cast<int>(cmd.mode));
  if (cmd.packageName) std::println(ofs, "packageName => {}", cmd.packageName.value());
  if (cmd.icon) std::println(ofs, "icon => {}", cmd.icon.value());
  if (cmd.iconDark) std::println(ofs, "iconDark => {}", cmd.iconDark.value());
  if (cmd.currentDirectoryPath)
    std::println(ofs, "currentDirectoryPath => {}", cmd.currentDirectoryPath.value().c_str());
  if (cmd.needsConfirmation) std::println(ofs, "needsConfirmation => {}", cmd.needsConfirmation);
  if (cmd.author) std::println(ofs, "author => {}", cmd.author.value());
  if (cmd.authorUrl) std::println(ofs, "authorUrl => {}", cmd.authorUrl.value());
  if (cmd.description) std::println(ofs, "description => {}", cmd.description.value());

  for (const auto &[idx, arg] : cmd.arguments | std::views::enumerate) {
    std::println(ofs, "argument{} => {}", idx, arg.placeholder.value_or("no placeholder"));
  }

  return ofs;
}

static std::expected<ScriptArgument, std::string> parseArgument(std::string_view json) {
  std::string buf{json};
  ParsedScriptArgument parsed;

  if (auto error = glz::read_json(parsed, buf)) {
    return std::unexpected(std::format("Failed to parse JSON: {}", glz::format_error(error)));
  }

  ScriptArgument result;

  if (parsed.type == "text") {
    result.type = ArgumentType::Text;
  } else if (parsed.type == "password") {
    result.type = ArgumentType::Password;
  } else if (parsed.type == "dropdown") {
    result.type = ArgumentType::Dropdown;
  } else {
    return std::unexpected(std::format("Unknown argument type: \"{}\"", parsed.type));
  }

  if (parsed.secure) { result.type = ArgumentType::Password; }

  if (result.type == ArgumentType::Dropdown && !parsed.data.has_value()) {
    return std::unexpected("Dropdown argument must have a 'data' field");
  }

  result.placeholder = std::move(parsed.placeholder);
  result.optional = parsed.optional.value_or(false);
  result.percentEncoded = parsed.percentEncoded.value_or(false);
  result.data = std::move(parsed.data);

  return result;
}

struct KV {
  std::string_view scope;
  std::string_view k;
  std::string_view v;
};

/**
 * Parse key value pair such as '@vicinae.icon = 🖌️'
 * Empty if the line cannot be parsed as such.
 */
static std::optional<KV> parseKV(std::string_view line) {
  static const auto scopes = {"@vicinae", "@raycast"};
  size_t i = 0;

  while (i < line.size() && std::isspace(line.at(i)))
    ++i;

  if (i == line.size() || line.at(i) != '@') return {};

  auto pos = line.find('.', i);

  if (pos == std::string::npos) return {};

  std::string_view scope = line.substr(i, pos - i);

  if (!std::ranges::contains(scopes, scope)) { return {}; }

  int keyStart = pos + 1;
  int keyEnd = keyStart;

  while (keyEnd < line.size() && !std::isspace(line.at(keyEnd))) {
    ++keyEnd;
  }

  std::string_view key = line.substr(keyStart, keyEnd - keyStart);

  int valueStart = keyEnd;

  while (valueStart < line.size() && std::isspace(line.at(valueStart))) {
    ++valueStart;
  }

  std::string_view value = line.substr(valueStart);

  return KV(scope, key, value);
}

static void trim(std::string_view &s, char c = ' ') {
  s.remove_prefix(std::min(s.find_first_not_of(c), s.size()));
  if (auto pos = s.find_last_not_of(c); pos != std::string::npos) { s.remove_suffix(s.size() - pos - 1); }
}

/**
 * Parse time string with unit suffix (s, m, h, d) to seconds
 * Examples: "5s" -> 5, "2m" -> 120, "1h" -> 3600, "1d" -> 86400
 */
static std::expected<std::uint64_t, std::string> parseTimeToSeconds(std::string_view timeStr) {
  if (timeStr.empty()) { return std::unexpected("Time string is empty"); }

  size_t unitPos = timeStr.size();
  for (size_t i = 0; i < timeStr.size(); ++i) {
    if (!std::isdigit(timeStr[i])) {
      unitPos = i;
      break;
    }
  }

  if (unitPos == 0) { return std::unexpected(std::format("Invalid time format: {}", timeStr)); }

  std::uint64_t value = 0;
  auto result = std::from_chars(timeStr.data(), timeStr.data() + unitPos, value);
  if (result.ec != std::errc{}) {
    return std::unexpected(std::format("Invalid number in time string: {}", timeStr));
  }

  std::string_view unit = timeStr.substr(unitPos);
  trim(unit);

  std::uint64_t multiplier = 1;
  if (unit.empty() || unit == "s") {
    multiplier = 1;
  } else if (unit == "m") {
    multiplier = 60;
  } else if (unit == "h") {
    multiplier = 3600;
  } else if (unit == "d") {
    multiplier = 86400;
  } else {
    return std::unexpected(std::format("Unknown time unit: {}", unit));
  }

  return value * multiplier;
}

std::expected<ScriptCommand, std::string> ScriptCommand::parse(std::string_view str) {
  // Note: Order matters! Check longer markers first to avoid false matches
  static const auto inlineCommentMarkers = {"//", "--", "#", ";"};
  ScriptCommand data;
  std::optional<std::string> scope;

  data.arguments.reserve(3);

  for (const auto &line : std::views::split(str, std::string_view{"\n"})) {
    std::string_view s{line};
    trim(s);

    auto marker =
        std::ranges::find_if(inlineCommentMarkers, [&](const char *marker) { return s.starts_with(marker); });

    if (marker != inlineCommentMarkers.end()) {
      const auto kv = parseKV(s.substr(std::string_view{*marker}.size()));

      if (!kv) continue;

      if (!scope) {
        scope = kv->scope;
      } else if (kv->scope != scope.value()) {
        return std::unexpected("Mixing @vicinae and @raycast keys is not allowed");
      }

      if (kv->k == "schemaVersion") {
        data.schemaVersion = kv->v;
      } else if (kv->k == "title") {
        data.title = kv->v;
      }

      if (kv->k == "mode") {
        auto mode = parseOutputMode(kv->v);
        if (!mode) { return std::unexpected(std::format("Invalid mode: \"{}\"", kv->v)); }
        data.mode = *mode;
      }

      if (kv->k == "icon") { data.icon = kv->v; }
      if (kv->k == "iconDark") { data.iconDark = kv->v; }
      if (kv->k == "packageName") { data.packageName = kv->v; }
      if (kv->k == "currentDirectoryPath") { data.currentDirectoryPath = kv->v; }
      if (kv->k == "needsConfirmation") {
        if (kv->v == "true") {
          data.needsConfirmation = true;
        } else if (kv->v == "false") {
          data.needsConfirmation = false;
        } else {
          return std::unexpected("needsConfirmation needs to be either true or false");
        }
      }
      if (kv->k == "author") { data.author = kv->v; }
      if (kv->k == "authorURL") { data.authorUrl = kv->v; }
      if (kv->k == "description") { data.description = kv->v; }
      if (kv->k == "refreshTime") {
        auto seconds = parseTimeToSeconds(kv->v);
        if (!seconds) {
          return std::unexpected(std::format("Failed to parse refreshTime: {}", seconds.error()));
        }
        data.refreshTime = seconds.value();
      }
      if (kv->k == "keywords") {
        if (kv->scope != "@vicinae") {
          return std::unexpected("keywords field is only supported in @vicinae scope");
        }
        std::string buf{kv->v};
        if (auto error = glz::read_json(data.keywords, buf)) {
          return std::unexpected(std::format("Failed to parse keywords: {}", glz::format_error(error)));
        }
      }
      if (kv->k == "exec") {
        if (kv->scope != "@vicinae") {
          return std::unexpected("exec field is only supported in @vicinae scope");
        }
        std::string buf{kv->v};
        if (auto error = glz::read_json(data.exec, buf)) {
          return std::unexpected(std::format("Failed to parse exec: {}", glz::format_error(error)));
        }
      }

      for (const char *argKey : {"argument1", "argument2", "argument3"}) {
        if (kv->k == argKey) {
          auto arg = parseArgument(kv->v);
          if (!arg) { return std::unexpected(std::format("Failed to parse {}: {}", argKey, arg.error())); }
          data.arguments.emplace_back(arg.value());
          break;
        }
      }
    }
  }

  if (data.schemaVersion != "1") { return std::unexpected("Invalid schema version, expected 1"); }
  if (data.title.empty()) { return std::unexpected("Title should not be empty"); }
  if (data.refreshTime.has_value() && data.mode != OutputMode::Inline) {
    return std::unexpected("refreshTime is only allowed when output mode is inline");
  }

  return data;
}

std::expected<ScriptCommand, std::string> ScriptCommand::fromFile(const std::filesystem::path &path) {
  std::ifstream ifs(path);
  return ScriptCommand::parse(ifs);
}

std::expected<ScriptCommand, std::string> ScriptCommand::parse(std::ifstream &ifs) {
  std::string buf;
  std::string line;
  // Note: Order matters! Check longer markers first to avoid false matches
  static const auto inlineCommentMarkers = {"//", "--", "#", ";"};

  while (std::getline(ifs, line)) {
    if (line.empty()) continue;

    std::string_view s{line};
    trim(s);

    bool startsWithComment =
        std::ranges::any_of(inlineCommentMarkers, [&](const char *marker) { return s.starts_with(marker); });

    buf.append(s);
    buf.append("\n");
  }

  return parse(buf);
}

} // namespace script_command
