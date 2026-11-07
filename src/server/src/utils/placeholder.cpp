#include "utils/placeholder.hpp"
#include <QUrl>
#include <QStringTokenizer>
#include <algorithm>
#include <cstdint>
#include <utility>

namespace placeholder_utils {

QDateTime applyDateOffset(QDateTime dt, QStringView offset) {
  offset = offset.trimmed();
  if (offset.size() < 3) return dt;
  QChar sign = offset.front();
  if (sign != '+' && sign != '-') return dt;
  QChar unit = offset.back();
  bool ok = false;
  int count = offset.mid(1, offset.size() - 2).toInt(&ok);
  if (!ok) return dt;
  if (sign == '-') count = -count;

  switch (unit.toLatin1()) {
  case 'm':
    return dt.addSecs(count * 60);
  case 'h':
  case 'H':
    return dt.addSecs(count * 3600);
  case 'd':
  case 'D':
    return dt.addDays(count);
  case 'w':
  case 'W':
    return dt.addDays(count * 7);
  case 'M':
    return dt.addMonths(count);
  case 'y':
  case 'Y':
    return dt.addYears(count);
  default:
    return dt;
  }
}

QDateTime applyDateOffsets(QDateTime dt, QStringView offsetStr) {
  for (const auto token : QStringTokenizer{offsetStr, u' '}) {
    if (token.isEmpty()) continue;
    dt = applyDateOffset(dt, token);
  }
  return dt;
}

QString normalizeDateFormat(QString fmt) {
  fmt.replace(QStringLiteral("YYYY"), QStringLiteral("yyyy"));
  fmt.replace(QStringLiteral("YY"), QStringLiteral("yy"));
  fmt.replace(QStringLiteral("DD"), QStringLiteral("dd"));
  return fmt;
}

QString applyModifier(QString val, QStringView mod) {
  auto m = mod.trimmed();
  if (m.compare(u"uppercase", Qt::CaseInsensitive) == 0) { return val.toUpper(); }
  if (m.compare(u"lowercase", Qt::CaseInsensitive) == 0) { return val.toLower(); }
  if (m.compare(u"trim", Qt::CaseInsensitive) == 0) { return val.trimmed(); }
  if (m.compare(u"percent-encode", Qt::CaseInsensitive) == 0 ||
      m.compare(u"url-encode", Qt::CaseInsensitive) == 0) {
    return QString::fromUtf8(QUrl::toPercentEncoding(val));
  }
  return val;
}

} // namespace placeholder_utils

PlaceholderString PlaceholderString::parseSnippetText(const QString &link) {
  return parse(link, std::vector<QString>{"uuid", "clipboard", "date", "cursor", "shell", "time", "datetime",
                                          "day", "selection", "selected"});
}

PlaceholderString PlaceholderString::parseShortcutText(const QString &link) {
  return parse(link, std::vector<QString>{"uuid", "clipboard", "selected", "selection", "date", "time",
                                          "datetime", "day"});
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
    PhValueQuoted,
    PhModifierStart,
    PhModifier,
    PhFallbackStart,
    PhFallback
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

      auto existing =
          std::ranges::find_if(pstr.m_args, [&](const Argument &a) { return a.name == arg.name; });
      if (existing == pstr.m_args.end()) {
        pstr.m_args.emplace_back(std::move(arg));
      } else if (existing->defaultValue.isEmpty() && !arg.defaultValue.isEmpty()) {
        existing->defaultValue = arg.defaultValue;
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
        parsed = {};
      }
      break;

    case BkEscape:
      if (ch == '\\' || ch == '{' || ch == '}') {
        pstr.m_parts.emplace_back(QString(ch));
        startPos = i + 1;
      } else {
        pstr.m_parts.emplace_back(QStringLiteral("\\"));
        startPos = i;
      }
      state = BkNormal;
      break;

    case PhId:
      if (ch == '}') {
        parsed.id = link.sliced(startPos, i - startPos).trimmed();
        pstr.m_parts.emplace_back(parsed);
        insertPlaceholder(parsed);
        parsed = {};
        startPos = i + 1;
        state = BkNormal;
      } else if (ch == '|') {
        parsed.id = link.sliced(startPos, i - startPos).trimmed();
        state = PhModifierStart;
        startPos = i + 1;
      } else if (ch == '?') {
        parsed.id = link.sliced(startPos, i - startPos).trimmed();
        state = PhFallbackStart;
      } else if (ch.isSpace() || !ch.isLetterOrNumber()) {
        parsed.id = link.sliced(startPos, i - startPos).trimmed();
        startPos = i;
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
      } else if (ch == '|') {
        state = PhModifierStart;
        startPos = i + 1;
      } else if (ch == '?') {
        state = PhFallbackStart;
      } else if (!ch.isSpace()) {
        startPos = i;
        arg.first.clear();
        arg.second.clear();
        state = PhKey;
      }
      break;

    case PhKey:
      if (ch == '=') {
        arg.first = link.sliced(startPos, i - startPos).trimmed();
        state = PhValueStart;
      } else if (ch == '|' || ch == '}' || ch == '?') {
        --i;
        state = PhKeyStart;
      }
      break;

    case PhValueStart:
      if (ch == '"') {
        startPos = i + 1;
        state = PhValueQuoted;
      } else if (!ch.isSpace()) {
        startPos = i;
        state = PhValue;
      }
      break;

    case PhValue:
      if (ch == ' ' || ch == '\t') {
        arg.second = link.sliced(startPos, i - startPos);
        parsed.args.insert(arg);
        state = PhKeyStart;
      } else if (ch == '}') {
        arg.second = link.sliced(startPos, i - startPos);
        parsed.args.insert(arg);
        pstr.m_parts.emplace_back(parsed);
        insertPlaceholder(parsed);
        parsed = {};
        startPos = i + 1;
        state = BkNormal;
      } else if (ch == '|') {
        arg.second = link.sliced(startPos, i - startPos);
        parsed.args.insert(arg);
        state = PhModifierStart;
        startPos = i + 1;
      } else if (ch == '?') {
        arg.second = link.sliced(startPos, i - startPos);
        parsed.args.insert(arg);
        state = PhFallbackStart;
      }
      break;

    case PhValueQuoted:
      if (ch == '"') {
        arg.second = link.sliced(startPos, i - startPos);
        parsed.args.insert(arg);
        state = PhKeyStart;
      }
      break;

    case PhModifierStart:
      if (ch == '}') {
        pstr.m_parts.emplace_back(parsed);
        insertPlaceholder(parsed);
        parsed = {};
        startPos = i + 1;
        state = BkNormal;
      } else if (ch == '|') {
        // skip duplicate pipe
      } else if (ch == '?') {
        state = PhFallbackStart;
      } else if (!ch.isSpace()) {
        startPos = i;
        state = PhModifier;
      }
      break;

    case PhModifier:
      if (ch == ' ' || ch == '\t') {
        auto mod = link.sliced(startPos, i - startPos).trimmed();
        if (!mod.isEmpty()) parsed.modifiers.emplace_back(mod);
        state = PhModifierStart;
      } else if (ch == '|') {
        auto mod = link.sliced(startPos, i - startPos).trimmed();
        if (!mod.isEmpty()) parsed.modifiers.emplace_back(mod);
        state = PhModifierStart;
        startPos = i + 1;
      } else if (ch == '?') {
        auto mod = link.sliced(startPos, i - startPos).trimmed();
        if (!mod.isEmpty()) parsed.modifiers.emplace_back(mod);
        state = PhFallbackStart;
      } else if (ch == '}') {
        auto mod = link.sliced(startPos, i - startPos).trimmed();
        if (!mod.isEmpty()) parsed.modifiers.emplace_back(mod);
        pstr.m_parts.emplace_back(parsed);
        insertPlaceholder(parsed);
        parsed = {};
        startPos = i + 1;
        state = BkNormal;
      }
      break;

    case PhFallbackStart:
      if (ch == '}') {
        pstr.m_parts.emplace_back(parsed);
        insertPlaceholder(parsed);
        parsed = {};
        startPos = i + 1;
        state = BkNormal;
      } else if (ch == '?') {
        // skip duplicate ?
      } else if (!ch.isSpace()) {
        startPos = i;
        state = PhFallback;
      }
      break;

    case PhFallback:
      if (ch == ' ' || ch == '\t') {
        parsed.fallbackId = link.sliced(startPos, i - startPos).trimmed();
        state = PhKeyStart;
      } else if (ch == '}') {
        parsed.fallbackId = link.sliced(startPos, i - startPos).trimmed();
        pstr.m_parts.emplace_back(parsed);
        insertPlaceholder(parsed);
        parsed = {};
        startPos = i + 1;
        state = BkNormal;
      } else if (ch == '|') {
        parsed.fallbackId = link.sliced(startPos, i - startPos).trimmed();
        state = PhModifierStart;
        startPos = i + 1;
      }
      break;
    }

    ++i;
  }

  if (state == BkNormal && i - startPos > 0) {
    pstr.m_parts.emplace_back(link.sliced(startPos, i - startPos));
  } else if (state == BkEscape) {
    pstr.m_parts.emplace_back(QStringLiteral("\\"));
  } else if (state != BkNormal && i - startPos > 0) {
    pstr.m_parts.emplace_back(link.sliced(startPos, i - startPos));
  }

  return pstr;
}
