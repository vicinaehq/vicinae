#include "placeholder.hpp"
#include <cstdint>
#include <utility>

PlaceholderString PlaceholderString::parseSnippetText(const QString &link) {
  return parse(link, std::vector<QString>{"uuid", "clipboard", "date", "cursor", "shell"});
}

PlaceholderString PlaceholderString::parseShortcutText(const QString &link) {
  return parse(link, std::vector<QString>{"uuid", "clipboard", "selected", "date"});
}

PlaceholderString PlaceholderString::parse(const QString &link, std::span<const QString> reserved) {
  PlaceholderString pstr;
  enum class State : std::uint8_t {
    BkNormal,
    BkEscape,
    PhId,
    PhKeyStart,
    PhKey,
    PhValueStart,
    PhValue,
    PhValueQuoted
  };
  using enum State;
  State state = BkNormal;
  size_t i = 0;
  size_t startPos = 0;
  ParsedPlaceholder parsed;
  std::pair<QString, QString> arg;

  const auto insertPlaceholder = [&](const ParsedPlaceholder &placeholder) {
    bool const isReserved =
        std::ranges::any_of(reserved, [&](const QString &s) { return s == placeholder.id; });
    bool const isArgument = !isReserved || placeholder.id == "argument";

    if (isArgument) {
      Argument arg;

      if (placeholder.id == "argument") {
        if (auto it = placeholder.args.find("name"); it != placeholder.args.end()) { arg.name = it->second; }
        if (auto it = placeholder.args.find("default"); it != placeholder.args.end()) {
          arg.defaultValue = it->second;
        }
      } else if (!isReserved) {
        arg.name = placeholder.id;
      }

      // an argument is always identified by its name and can be expanded in multiple places
      if (!std::ranges::any_of(pstr.m_args, [&](auto &&a) { return arg.name == a.name; })) {
        pstr.m_args.emplace_back(arg);
      }
    }

    pstr.m_placeholders.emplace_back(placeholder);
  };

  pstr.m_parts.clear();
  pstr.m_placeholders.clear();
  pstr.m_args.clear();
  pstr.m_raw = link;

  while (std::cmp_less(i, link.size())) {
    QChar const ch = link.at(i);

    switch (state) {
    case BkNormal:
      if (ch == '\\') {
        pstr.m_parts.emplace_back(link.sliced(startPos, i - startPos));
        state = BkEscape;
        break;
      }
      if (ch == '{') {
        pstr.m_parts.emplace_back(link.sliced(startPos, i - startPos));
        state = PhId;
        startPos = i + 1;
      }
      break;
    case BkEscape:
      if (ch == '\\') {
        pstr.m_parts.emplace_back(QStringLiteral("\\"));
        startPos = i + 1;
      } else {
        startPos = i;
      }
      state = BkNormal;
      break;
    case PhId:
      if (!ch.isLetterOrNumber()) {
        parsed.id = link.sliced(startPos, i - startPos);
        startPos = i--;
        state = PhKeyStart;
      }
      break;
    case PhKeyStart:
      if (ch == '}') {
        pstr.m_parts.emplace_back(parsed);
        insertPlaceholder(parsed);
        parsed = {};
        startPos = i + 1;
        state = BkNormal;
        break;
      }
      if (!ch.isSpace()) {
        startPos = i--;
        arg.first.clear();
        arg.second.clear();
        state = PhKey;
      }
      break;
    case PhKey:
      if (ch == '=') {
        arg.first = link.sliced(startPos, i - startPos);
        state = PhValueStart;
      }
      break;
    case PhValueStart:
      if (!ch.isSpace()) {
        startPos = i--;
        state = PhValue;
      }
      break;
    case PhValue:
      if (ch == '"') {
        arg.second += link.sliced(startPos, i - startPos);
        startPos = i + 1;
        state = PhValueQuoted;
        break;
      }
      if (!ch.isLetterOrNumber()) {
        arg.second += link.sliced(startPos, i - startPos);
        parsed.args.insert(arg);
        --i;
        state = PhKeyStart;
      }
      break;
    case PhValueQuoted:
      if (ch == '"') {
        arg.second += link.sliced(startPos, i - startPos);
        startPos = i + 1;
        state = PhValue;
      }
    }

    ++i;
  }

  if (state == BkNormal && i - startPos > 0) {
    pstr.m_parts.emplace_back(link.sliced(startPos, i - startPos));
  } else if (state == BkEscape) {
    pstr.m_parts.emplace_back(QStringLiteral("\\"));
  }

  return pstr;
}
