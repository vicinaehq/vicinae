#pragma once
#include "../locale/locale.hpp"
#include "group.hpp"
#include <cctype>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace xdgpp {
namespace {};

static constexpr const char LF = '\n';

/**
 * A reader/parser for the desktop entry file format.
 * This level of abstraction only understands the concept of groups, keys and
 * values.
 *
 * It will also applies relevant escaping rules for the different value
 * types that are supported by the specification. This will not give you direct
 * high level functions to query the name of the application or anything like
 * that.
 *
 * If that's what you want, you should use the higher level `DesktopEntry`
 * class.
 */
class DesktopEntryReader {
  using Group = DesktopEntryGroup;

public:
  struct DesktopEntryReaderSettings {
    std::optional<Locale> locale;
  };

  /**
   * Parses the desktop entry passed as `data`.
   */
  DesktopEntryReader(std::string_view data, const DesktopEntryReaderSettings &opts = {}) : m_data(data) {
    m_locale = opts.locale.value_or(Locale::system());
    parse();
  }

  const Group *group(std::string_view key) const {
    if (auto it = m_groups.find(std::string(key)); it != m_groups.end()) { return it->second.get(); }
    return nullptr;
  }

  std::vector<std::string> groupNames() const {
    std::vector<std::string> names;
    for (const auto &[k, v] : m_groups) {
      names.emplace_back(v->name());
    }
    return names;
  }

  const Locale &locale() const { return m_locale; }

private:
  static bool isGroupHeaderChar(char c) { return c != '[' && c != ']' && c != '\n'; }
  static bool isInlineSpace(char c) { return c == ' ' || c == '\t' || c == '\v' || c == '\r' || c == '\f'; }
  static bool isKeyChar(char c) { return c != '=' && c != LF && c != '[' && !std::isspace(c); }

  // [Desktop Entry]
  void parseGroupHeader();
  std::string parseKey();
  std::string parseRawValue();
  void skipSpace();

  // consumes only if current character is c
  void consume(char c);
  char consume();
  char peek() const;
  bool isPeek(char c) const;

  // lang_COUNTRY.ENCODING@MODIFIER
  std::string parseRawLocale();
  // https://specifications.freedesktop.org/desktop-entry-spec/latest/localized-keys.html
  size_t computeLocalScore(const Locale &locale);
  void parseEntry();
  void parse();

  size_t m_cursor = 0;
  std::string m_data;
  std::unordered_map<std::string, std::unique_ptr<Group>> m_groups;
  Group *m_currentGroup = nullptr;
  Locale m_locale;

  std::vector<std::string> warnings;
};
}; // namespace xdgpp
