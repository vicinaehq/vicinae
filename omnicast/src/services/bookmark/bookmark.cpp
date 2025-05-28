#include "services/bookmark/bookmark.hpp"

void Bookmark::insertPlaceholder(const ParsedPlaceholder &placeholder) {
  bool isReserved =
      std::ranges::any_of(m_reservedPlaceholderIds, [&](const QString &s) { return s == placeholder.id; });
  bool isArgument = !isReserved || placeholder.id == "argument";

  if (isArgument) {
    Argument arg;

    if (!isReserved) arg.name = placeholder.id;
    if (auto it = placeholder.args.find("name"); it != placeholder.args.end()) { arg.name = it->second; }
    if (auto it = placeholder.args.find("default"); it != placeholder.args.end()) {
      arg.defaultValue = it->second;
    }

    m_args.emplace_back(arg);
  }

  m_placeholders.emplace_back(placeholder);
}

QString Bookmark::app() const { return m_app; }
QString Bookmark::name() const { return m_name; }
QString Bookmark::icon() const { return m_icon; }
std::vector<Bookmark::UrlPart> Bookmark::parts() const { return m_parts; }
const std::vector<Bookmark::ParsedPlaceholder> &Bookmark::placeholders() const { return m_placeholders; }
const std::vector<Bookmark::Argument> &Bookmark::arguments() const { return m_args; }

int Bookmark::id() const { return m_id; }
QString Bookmark::url() const { return m_raw; }

void Bookmark::setApp(const QString &app) { m_app = app; }
void Bookmark::setName(const QString &name) { m_name = name; }
void Bookmark::setIcon(const QString &icon) { m_icon = icon; }
void Bookmark::setId(int id) { m_id = id; }

void Bookmark::setLink(const QString &link) {
  enum {
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

  m_placeholders.clear();
  m_args.clear();
  m_raw = link;

  while (i < link.size()) {
    QChar ch = link.at(i);

    switch (state) {
    case BK_NORMAL:
      if (ch == '{') {
        m_parts.emplace_back(link.sliced(startPos, i - startPos));
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
        m_parts.emplace_back(parsed);
        insertPlaceholder(parsed);
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
        qDebug() << "key" << arg.first;
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
        qDebug() << "value" << arg.second;
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

  if (state == BK_NORMAL && i - startPos > 0) { m_parts.emplace_back(link.sliced(startPos, i - startPos)); }
}
