#include "services/shortcut/shortcut.hpp"

#include <utility>

void Shortcut::insertPlaceholder(const ParsedPlaceholder &placeholder) {
  bool const isReserved =
      std::ranges::any_of(m_reservedPlaceholderIds, [&](const QString &s) { return s == placeholder.id; });
  bool const isArgument = !isReserved || placeholder.id == "argument";

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

QString Shortcut::app() const { return m_app; }
QString Shortcut::name() const { return m_name; }
QString Shortcut::icon() const { return m_icon; }
std::vector<Shortcut::UrlPart> Shortcut::parts() const { return m_parts; }
const std::vector<Shortcut::ParsedPlaceholder> &Shortcut::placeholders() const { return m_placeholders; }
const std::vector<Shortcut::Argument> &Shortcut::arguments() const { return m_args; }

QString Shortcut::id() const { return m_id; }
QString Shortcut::url() const { return m_raw; }

int Shortcut::openCount() const { return m_openCount; }
QDateTime Shortcut::createdAt() { return m_createdAt; }
QDateTime Shortcut::updatedAt() { return m_updatedAt; }
std::optional<QDateTime> Shortcut::lastOpenedAt() { return m_lastOpenedAt; }

void Shortcut::setApp(const QString &app) { m_app = app; }
void Shortcut::setName(const QString &name) { m_name = name; }
void Shortcut::setIcon(const QString &icon) { m_icon = icon; }
void Shortcut::setId(const QString &id) { m_id = id; }

void Shortcut::setCreatedAt(const QDateTime &date) { m_createdAt = date; }
void Shortcut::setUpdatedAt(const QDateTime &date) { m_updatedAt = date; }
void Shortcut::setLastOpenedAt(const std::optional<QDateTime> &date) { m_lastOpenedAt = date; }
void Shortcut::setOpenCount(int openCount) { m_openCount = openCount; }

void Shortcut::parseLink(const QString &link) {
  enum class State {
    BkNormal,
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

  m_parts.clear();
  m_placeholders.clear();
  m_args.clear();
  m_raw = link;

  while (std::cmp_less(i,  link.size())) {
    QChar const ch = link.at(i);

    switch (state) {
    case BkNormal:
      if (ch == '{') {
        m_parts.emplace_back(link.sliced(startPos, i - startPos));
        state = PhId;
        startPos = i + 1;
      }
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
        m_parts.emplace_back(parsed);
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

  if (state == BkNormal && i - startPos > 0) { m_parts.emplace_back(link.sliced(startPos, i - startPos)); }
}
