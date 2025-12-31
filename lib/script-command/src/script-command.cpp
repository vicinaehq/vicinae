#include "script-command.hpp"
#include <algorithm>
#include <charconv>
#include <format>
#include <fstream>
#include <glaze/json/read.hpp>
#include <iostream>
#include <ranges>

namespace script_command {

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
    std::println(ofs, "argument{} => {}", idx, arg.placeholder);
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

  if (!std::ranges::contains(scopes, scope)) {
    std::println(std::cerr, "Unknown scope in comment: {}", scope);
    return {};
  }

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

namespace {
void trim(std::string_view &s, char c = ' ') {
  s.remove_prefix(std::min(s.find_first_not_of(c), s.size()));
  if (auto pos = s.find_last_not_of(c); pos != std::string::npos) { s.remove_suffix(s.size() - pos - 1); }
}

/**
 * Parse shebang line and extract exec command and arguments
 * Example: "#!/bin/bash -c hello" -> ["bin/bash", "-c", "hello"]
 */
std::optional<std::vector<std::string>> parseShebang(std::string_view line) {
  std::string_view s = line;
  trim(s);

  if (!s.starts_with("#!")) { return {}; }

  s.remove_prefix(2);
  trim(s);

  if (s.empty()) { return {}; }

  auto tokens = s | std::views::split(' ') |
                std::views::filter([](auto &&token) { return !std::ranges::empty(token); }) |
                std::views::transform([](auto &&token) { return std::string(std::string_view(token)); }) |
                std::ranges::to<std::vector>();

  return tokens;
}

/**
 * Parse time string with unit suffix (s, m, h, d) to seconds
 * Examples: "5s" -> 5, "2m" -> 120, "1h" -> 3600, "1d" -> 86400
 */
std::expected<std::uint64_t, std::string> parseTimeToSeconds(std::string_view timeStr) {
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

} // namespace

std::expected<ScriptCommand, std::string> ScriptCommand::parse(std::string_view str) {
  static const auto inlineCommentMarkers = {"#", "//"};
  ScriptCommand data;
  std::optional<std::string> scope;
  bool firstLine = true;

  data.arguments.reserve(3);

  for (const auto &line : std::views::split(str, std::string_view{"\n"})) {
    std::string_view s{line};
    trim(s);

    if (firstLine) {
      firstLine = false;
      if (auto exec = parseShebang(s)) {
        data.exec = std::move(exec.value());
        continue;
      }
    }

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
        if (kv->v == "fullOutput") {
          data.mode = OutputMode::Full;
        } else if (kv->v == "compact") {
          data.mode = OutputMode::Compact;
        } else if (kv->v == "silent") {
          data.mode = OutputMode::Silent;
        } else if (kv->v == "inline") {
          data.mode = OutputMode::Inline;
        } else if (kv->v == "terminal") {
          data.mode = OutputMode::Terminal;
        } else {
          return std::unexpected(std::format("Invalid mode: \"{}\"", kv->v));
        }
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

  if (data.exec.empty()) { return std::unexpected("Shebang is required"); }
  if (data.schemaVersion != "1") { return std::unexpected("Invalid schema version, expected 1"); }
  if (data.title.empty()) { return std::unexpected("Title should not be empty"); }
  if (data.refreshTime.has_value() && data.mode != OutputMode::Inline) {
    return std::unexpected("refreshTime is only allowed when output mode is inline");
  }
  if (data.mode == OutputMode::Inline && !data.refreshTime.has_value()) {
    return std::unexpected("refreshTime is required when output mode is inline");
  }

  return data;
}

std::expected<ScriptCommand, std::string> ScriptCommand::parse(std::ifstream &ifs) {
  std::string buf;
  std::string line;
  static const auto inlineCommentMarkers = {"#", "//"};

  while (std::getline(ifs, line)) {
    if (line.empty()) continue;

    std::string_view s{line};
    trim(s);

    bool startsWithComment =
        std::ranges::any_of(inlineCommentMarkers, [&](const char *marker) { return s.starts_with(marker); });

    if (!startsWithComment) break;

    buf.append(s);
    buf.append("\n");
  }

  return parse(buf);
}

} // namespace script_command
