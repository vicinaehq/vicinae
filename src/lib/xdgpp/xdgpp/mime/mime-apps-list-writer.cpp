#include "mime-apps-list-writer.hpp"
#include <algorithm>
#include <fstream>
#include <functional>
#include <optional>
#include <ranges>
#include <string>
#include <vector>
#include "../env/env.hpp"
#include "../utils/utils.hpp"

namespace fs = std::filesystem;

namespace {

using Lines = std::vector<std::string>;

constexpr std::string_view DEFAULT_APPLICATIONS_GROUP = "Default Applications";
constexpr std::string_view ADDED_ASSOCIATIONS_GROUP = "Added Associations";

bool isGroupHeader(std::string_view line) {
  const auto entry = trim(line);
  return entry.starts_with('[') && entry.ends_with(']');
}

std::string_view keyOf(std::string_view line) {
  const auto entry = trim(line);
  return trim(entry.substr(0, entry.find('=')));
}

std::string_view valueOf(std::string_view line) {
  const auto entry = trim(line);
  const auto eq = entry.find('=');
  return eq == std::string_view::npos ? std::string_view{} : trim(entry.substr(eq + 1));
}

/**
 * Locate the body of the group as a [begin, end) line range, appending an empty group if it is missing.
 * Blank lines separating the group from the next one are excluded from the range.
 */
std::pair<std::size_t, std::size_t> findGroup(Lines &lines, std::string_view name) {
  const std::string header = std::string("[").append(name).append("]");
  const auto it = std::ranges::find_if(lines, [&](const auto &line) { return trim(line) == header; });

  if (it == lines.end()) {
    if (!lines.empty() && !trim(lines.back()).empty()) lines.emplace_back();
    lines.emplace_back(header);
    return {lines.size(), lines.size()};
  }

  const std::size_t begin = std::distance(lines.begin(), it) + 1;
  std::size_t end = begin;

  while (end < lines.size() && !isGroupHeader(lines[end]))
    ++end;
  while (end > begin && trim(lines[end - 1]).empty())
    --end;

  return {begin, end};
}

void setKey(Lines &lines, std::string_view group, std::string_view key,
            const std::function<std::string(std::optional<std::string_view>)> &makeValue) {
  const auto [begin, end] = findGroup(lines, group);
  const auto makeLine = [&](std::optional<std::string_view> current) {
    return std::string(key).append("=").append(makeValue(current));
  };

  const auto matches = [&](const auto &line) { return keyOf(line) == key; };
  const auto first = std::find_if(lines.begin() + begin, lines.begin() + end, matches);

  if (first == lines.begin() + end) {
    lines.insert(lines.begin() + end, makeLine(std::nullopt));
    return;
  }

  *first = makeLine(valueOf(*first));
  lines.erase(std::remove_if(first + 1, lines.begin() + end, matches), lines.begin() + end);
}

std::string prependToList(std::optional<std::string_view> current, std::string_view appId) {
  auto ids = current.value_or("") | std::views::split(';') |
             std::views::transform([](auto &&part) { return trim(std::string_view(part)); }) |
             std::views::filter([](auto part) { return !part.empty(); }) | std::ranges::to<std::vector>();

  if (!std::ranges::contains(ids, appId)) ids.insert(ids.begin(), appId);

  std::string value;
  for (const auto id : ids) {
    value.append(id).append(";");
  }

  return value;
}

Lines readLines(const fs::path &path) {
  Lines lines;
  std::ifstream ifs{path};
  std::string line;

  while (std::getline(ifs, line)) {
    lines.emplace_back(std::move(line));
  }

  return lines;
}

bool writeLines(const fs::path &path, const Lines &lines) {
  std::error_code ec;
  fs::path tmp = path;

  tmp += ".tmp";
  fs::create_directories(path.parent_path(), ec);

  {
    std::ofstream ofs{tmp};
    for (const auto &line : lines) {
      ofs << line << '\n';
    }
    if (!ofs.good()) return false;
  }

  fs::rename(tmp, path, ec);

  return !ec;
}

} // namespace

namespace xdgpp {

bool setDefaultApplication(std::span<const std::string_view> mimes, std::string_view appId,
                           const fs::path &path) {
  auto lines = readLines(path);

  for (const auto mime : mimes) {
    setKey(lines, DEFAULT_APPLICATIONS_GROUP, mime, [&](auto) { return std::string(appId); });
    setKey(lines, ADDED_ASSOCIATIONS_GROUP, mime,
           [&](auto current) { return prependToList(current, appId); });
  }

  return writeLines(path, lines);
}

bool setDefaultApplication(std::span<const std::string_view> mimes, std::string_view appId) {
  return setDefaultApplication(mimes, appId, configHome() / "mimeapps.list");
}

}; // namespace xdgpp
