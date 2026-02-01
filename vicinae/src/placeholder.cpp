#include "placeholder.hpp"
#include <cstdint>

PlaceholderString PlaceholderString::parseSnippetText(QString link) {
  return parse(link, std::vector<QString>{"uuid", "clipboard", "date", "cursor"});
}

PlaceholderString PlaceholderString::parseShortcutText(QString link) {
  return parse(link, std::vector<QString>{"uuid", "clipboard", "selected", "date"});
}

PlaceholderString PlaceholderString::parse(QString link, std::span<const QString> reserved) {
  PlaceholderString pstr;
  enum : std::uint8_t {
    BK_NORMAL,
    PH_ID,
    PH_KEY_START,
    PH_KEY,
    PH_VALUE_START,
    PH_VALUE,
    PH_VALUE_QUOTED
  } state = BK_NORMAL;
  size_t i = 0;
  size_t startPos = 0;
  ParsedPlaceholder parsed;
  std::pair<QString, QString> arg;

  const auto insertPlaceholder = [&](const ParsedPlaceholder &placeholder) {
    bool isReserved = std::ranges::any_of(reserved, [&](const QString &s) { return s == placeholder.id; });

    if (placeholder.id == "argument") {
      Argument arg;
      if (auto it = placeholder.args.find("name"); it != placeholder.args.end()) { arg.name = it->second; }
      if (auto it = placeholder.args.find("default"); it != placeholder.args.end()) {
        arg.defaultValue = it->second;
      }

      pstr.m_args.emplace_back(arg);
    } else if (!isReserved) {
      pstr.m_args.emplace_back(Argument(placeholder.id));
    }

    pstr.m_placeholders.emplace_back(placeholder);
  };

  pstr.m_parts.clear();
  pstr.m_placeholders.clear();
  pstr.m_args.clear();
  pstr.m_raw = link;

  while (i < link.size()) {
    QChar ch = link.at(i);

    switch (state) {
    case BK_NORMAL:
      if (ch == '{') {
        pstr.m_parts.emplace_back(link.sliced(startPos, i - startPos));
        state = PH_ID;
        startPos = i + 1;
      }
      break;
    case PH_ID:
      if (!ch.isLetterOrNumber()) {
        parsed.id = link.sliced(startPos, i - startPos);
        startPos = i--;
        state = PH_KEY_START;
      }
      break;
    case PH_KEY_START:
      if (ch == '}') {
        pstr.m_parts.emplace_back(parsed);
        insertPlaceholder(parsed);
        parsed = {};
        startPos = i + 1;
        state = BK_NORMAL;
        break;
      }
      if (!ch.isSpace()) {
        startPos = i--;
        arg.first.clear();
        arg.second.clear();
        state = PH_KEY;
      }
      break;
    case PH_KEY:
      if (ch == '=') {
        arg.first = link.sliced(startPos, i - startPos);
        state = PH_VALUE_START;
      }
      break;
    case PH_VALUE_START:
      if (!ch.isSpace()) {
        startPos = i--;
        state = PH_VALUE;
      }
      break;
    case PH_VALUE:
      if (ch == '"') {
        arg.second += link.sliced(startPos, i - startPos);
        startPos = i + 1;
        state = PH_VALUE_QUOTED;
        break;
      }
      if (!ch.isLetterOrNumber()) {
        arg.second += link.sliced(startPos, i - startPos);
        parsed.args.insert(arg);
        --i;
        state = PH_KEY_START;
      }
      break;
    case PH_VALUE_QUOTED:
      if (ch == '"') {
        arg.second += link.sliced(startPos, i - startPos);
        startPos = i + 1;
        state = PH_VALUE;
      }
    }

    ++i;
  }

  if (state == BK_NORMAL && i - startPos > 0) {
    pstr.m_parts.emplace_back(link.sliced(startPos, i - startPos));
  }

  return pstr;
}
