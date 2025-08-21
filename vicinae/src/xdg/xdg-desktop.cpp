#include "xdg-desktop.hpp"
#include <qnamespace.h>
#include <qstringview.h>

static bool isGroupHeaderNameChar(QChar c) { return c.isPrint() && c != '[' && c != ']'; }

static bool isEntryKey(QChar c) { return c.isLetterOrNumber() || c == '-'; }

static bool isValueKey(QChar c) { return c != '\n'; }

static bool isLocaleKey(QChar c) { return c.isLetter() || c == '_' || c == '@' || c == '-'; }

Locale::Locale(QStringView data) {
  enum State { LANG, COUNTRY, ENC, AT, MOD, DONE };
  State state = LANG;

  size_t cursor = 0;
  size_t i = 0;
  size_t end = i;

  while (state != DONE && i < data.size()) {
    QChar c = data.at(i);

    switch (state) {
    case LANG:
      if (c.isLetter()) {
        ++end;
        break;
      }

      lang = data.sliced(cursor, end - cursor);
      cursor = end = i + 1;

      if (c == '_' || c == '-') {
        state = COUNTRY;
      } else if (c == '.') {
        state = ENC;
      } else if (c == '@') {
        state = AT;
      } else {
        state = DONE;
      }
      break;
    case COUNTRY:
      if (c.isLetter()) {
        ++end;
        break;
      }

      country = data.sliced(cursor, end - cursor);
      cursor = end = i + 1;

      if (c == '.') {
        state = ENC;
      } else if (c == '-') {
        state = MOD;
      } else if (c == '@') {
        state = AT;
      } else {
        state = DONE;
      }

      break;
    case ENC:
      if (c.isLetterOrNumber() || c == '-') {
        ++end;
        break;
      }

      encoding = data.sliced(cursor, end - cursor);
      cursor = end = i + 1;

      if (c == '@') {
        state = AT;
      } else {
        state = DONE;
      }

      break;
    case AT:
      if (c.isLetter()) {
        ++end;
        break;
      }

      modifier = data.sliced(cursor, end - cursor);
      cursor = end = i + 1;
      state = DONE;

      break;
    case MOD:
      if (c.isLetter()) {
        ++end;
        break;
      }

      variant = data.sliced(cursor, end - cursor);
      cursor = end = i + 1;

      if (c == '@') {
        state = AT;
      } else {
        state = DONE;
      }

      break;
    default:
      break;
    }

    ++i;
  }
}

QString Locale::toString() {
  QString fmt;

  if (!lang.isEmpty()) fmt += lang;

  if (!country.isEmpty()) {
    fmt += "_";
    fmt += country;
  }

  if (!variant.isEmpty()) {
    fmt += "-";
    fmt += variant;
  }

  if (!encoding.isEmpty()) {
    fmt += ".";
    fmt += encoding;
  }

  if (!modifier.isEmpty()) {
    fmt += "@";
    fmt += modifier;
  }
  struct Entry {
    QStringView key;
    QString value;
    std::optional<Locale> locale;
  };
  return fmt;
}

XdgDesktopEntry::Parser::Parser(QStringView view) noexcept : rawLocale("C"), data(view), cursor(0) {
  if (auto locale = std::setlocale(LC_MESSAGES, nullptr)) { rawLocale = locale; }
}

void XdgDesktopEntry::Parser::skipWS() {
  while (cursor < data.size() && data.at(cursor).isSpace())
    ++cursor;
}

QStringView XdgDesktopEntry::Parser::parseGroupHeader() {
  size_t end = cursor;

  while (isGroupHeaderNameChar(data.at(end))) {
    ++end;
  }

  QStringView view = data.sliced(cursor, end - cursor);

  if (data.at(end) != ']') throw std::runtime_error("Invalid group header");

  cursor = end + 1;

  return view;
}

QStringView XdgDesktopEntry::Parser::parseEntryKey() {
  size_t end = cursor;

  while (isEntryKey(data.at(end)))
    ++end;

  auto key = data.sliced(cursor, end - cursor);

  cursor = end;

  return key;
}

QChar XdgDesktopEntry::Parser::parseEscaped() {
  QChar c = data.at(cursor++);

  switch (c.row()) {
    // clang-format off
	  case 's': return ' ';
	  case 'n': return '\n';
	  case 't': return '\t';
	  case 'r': return '\r';
	  default: return c;
    // clang-format on
  }
}

QString XdgDesktopEntry::Parser::parseEntryValue() {
  skipWS();

  QString value;
  QChar c;

  while (cursor < data.size()) {
    c = data.at(cursor);

    if (!isValueKey(c)) { break; }

    if (c == '\\') {
      cursor++;
      value += parseEscaped();
    } else {
      value += c;
      cursor++;
    }
  }

  ++cursor;

  return value;
}

XdgDesktopEntry::Parser::Entry XdgDesktopEntry::Parser::parseEntry() {
  Entry entry;

  skipWS();
  entry.key = parseEntryKey();

  if (data.at(cursor) == '[') {
    size_t end = ++cursor;

    while (end < data.size() && data.at(end) != ']')
      ++end;

    auto view = data.sliced(cursor, end - cursor);

    entry.locale = {view};
    cursor = end + 1;
  }

  skipWS();

  if (cursor < data.size() && data.at(cursor) != '=') { throw std::runtime_error("Invalid key name"); }

  cursor++;
  entry.value = parseEntryValue();

  return entry;
}

uint XdgDesktopEntry::Parser::computeLocalePriority(Locale lhs, Locale rhs) {
  if (lhs.lang == rhs.lang) {
    if (lhs.country == rhs.country) {
      if (lhs.modifier == rhs.modifier) return 3;
      return 2;
    }
    return 1;
  }

  return 0;
}

XdgDesktopEntry XdgDesktopEntry::Parser::parse() {
  QHash<QStringView, size_t> localePriorities;

  skipWS();

  while (cursor < data.size()) {
    QChar ch = data.at(cursor);

    if (ch == '#') {
      while (data.at(cursor) != '\n')
        ++cursor;
    }

    else if (ch == '[') {
      ++cursor;
      auto groupName = parseGroupHeader();
      auto it = groups.insert(groupName, {});

      currentGroup = &*it;
      localePriorities.clear();
    }

    else {
      auto entry = parseEntry();

      auto previousScore = localePriorities.find(entry.key);

      if (!entry.locale && previousScore == localePriorities.end()) {
        localePriorities.insert(entry.key, 0);
        currentGroup->insert(entry.key, entry.value);
      }

      if (entry.locale) {
        size_t score = computeLocalePriority(Locale(rawLocale), *entry.locale);

        if (previousScore != localePriorities.end() && *previousScore < score) {
          // qDebug() << "out did local score for " << entry.key << entry.locale->toString() << rawLocale;
          *previousScore = score;

          currentGroup->insert(entry.key, entry.value);
        }

        if (previousScore == localePriorities.end() && score > 0) {
          localePriorities.insert(entry.key, score);
          currentGroup->insert(entry.key, entry.value);
        }
      }
    }

    skipWS();
  }

  XdgDesktopEntry entry;

  auto it = groups.find(QStringLiteral("Desktop Entry"));

  if (it == groups.end()) throw std::runtime_error("No Desktop Entry group");

  entry.type = (*it)[QStringLiteral("Type")];
  entry.version = (*it)[QStringLiteral("Version")];
  entry.name = (*it)[QStringLiteral("Name")];
  entry.genericName = (*it)[QStringLiteral("GenericName")];
  entry.noDisplay = (*it)[QStringLiteral("NoDisplay")] == "true";
  entry.comment = (*it)[QStringLiteral("Comment")];
  entry.icon = (*it)[QStringLiteral("Icon")];
  entry.hidden = (*it)[QStringLiteral("Hidden")] == "true";
  entry.tryExec = (*it)[QStringLiteral("TryExec")];
  entry.exec = ExecParser::parse((*it)[QStringLiteral("Exec")]);

  entry.path = (*it)[QStringLiteral("Path")];
  entry.terminal = (*it)[QStringLiteral("Terminal")] == "true";
  auto actions = (*it)[QStringLiteral("Actions")].split(';', Qt::SkipEmptyParts);
  entry.mimeType = (*it)[QStringLiteral("MimeType")].split(';', Qt::SkipEmptyParts);
  entry.categories = (*it)[QStringLiteral("Categories")].split(';', Qt::SkipEmptyParts);
  entry.keywords = (*it)[QStringLiteral("Keywords")].split(';', Qt::SkipEmptyParts);
  entry.startupWMClass = (*it)[QStringLiteral("StartupWMClass")];
  entry.singleMainWindow = (*it)[QStringLiteral("SingleMainWindow")] == "true";

  for (const auto &groupName : groups.keys()) {
    auto parts = groupName.split(' ');

    if (parts.size() == 3 && parts[0] == QStringLiteral("Desktop") && parts[1] == QStringLiteral("Action") &&
        actions.contains(parts[2])) {

      XdgDesktopEntry::Action action;

      auto group = groups[groupName];

      action.id = parts[2].toString();
      action.name = group[QStringLiteral("Name")];
      action.icon = group[QStringLiteral("Icon")];
      action.exec = ExecParser::parse(group[QStringLiteral("Exec")]);
      entry.actions.push_back(action);
    }
  }

  return entry;
}
