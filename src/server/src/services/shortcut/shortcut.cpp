#include "services/shortcut/shortcut.hpp"
#include "services/selection/abstract-selection-service.hpp"
#include "service-registry.hpp"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QClipboard>
#include <QUuid>
#include <QUrl>
#include <unordered_map>
#include <algorithm>
#include <utility>

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
  auto pstr = PlaceholderString::parse(link, m_reservedPlaceholderIds);
  m_raw = link;
  m_placeholders.assign(pstr.placeholders().begin(), pstr.placeholders().end());
  m_args.assign(pstr.arguments().begin(), pstr.arguments().end());
  m_parts.assign(pstr.parts().begin(), pstr.parts().end());
}

QString Shortcut::expand(std::span<const QString> args) const {
  QString expanded;
  const bool isUrl = m_raw.contains(QStringLiteral("://"));

  std::unordered_map<QString, QString> argValues;
  for (size_t idx = 0; idx < m_args.size(); ++idx) {
    const auto &argDef = m_args[idx];
    if (idx < args.size() && !args[idx].isEmpty()) {
      argValues[argDef.name] = args[idx];
    } else if (!argDef.defaultValue.isEmpty()) {
      argValues[argDef.name] = argDef.defaultValue;
    } else {
      argValues[argDef.name] = QString();
    }
  }

  for (const auto &part : m_parts) {
    if (const auto *s = std::get_if<QString>(&part)) {
      expanded += *s;
      continue;
    }

    const auto *ph = std::get_if<ParsedPlaceholder>(&part);
    if (!ph) continue;

    QString val;
    bool isArg = false;

    if (ph->id == "clipboard") {
      if (auto *app = QCoreApplication::instance()) {
        if (auto *gui = qobject_cast<QGuiApplication *>(app)) {
          if (auto *clip = gui->clipboard()) val = clip->text();
        }
      }
    } else if (ph->id == "selected" || ph->id == "selection") {
      if (auto *reg = ServiceRegistry::instance()) {
        if (auto selService = reg->selectionService()) {
          if (auto selected = selService->selectedTextSync()) { val = *selected; }
        }
      }
    } else if (ph->id == "uuid") {
      val = QUuid::createUuid().toString(QUuid::WithoutBraces);
    } else if (ph->id == "date" || ph->id == "datetime" || ph->id == "time" || ph->id == "day") {
      QDateTime dt = QDateTime::currentDateTime();
      if (auto it = ph->args.find("offset"); it != ph->args.end()) {
        dt = placeholder_utils::applyDateOffsets(dt, it->second);
      }
      QString fmt;
      if (auto it = ph->args.find("format"); it != ph->args.end()) {
        fmt = placeholder_utils::normalizeDateFormat(it->second);
      } else if (ph->id == "time") {
        fmt = QStringLiteral("HH:mm");
      } else if (ph->id == "day") {
        fmt = QStringLiteral("dddd");
      } else if (ph->id == "datetime") {
        fmt = QStringLiteral("yyyy-MM-dd HH:mm:ss");
      } else {
        fmt = QStringLiteral("yyyy-MM-dd");
      }
      val = dt.toString(fmt);
    } else {
      isArg = true;
      QString name = ph->id;
      if (ph->id == "argument") {
        if (auto it = ph->args.find("name"); it != ph->args.end()) { name = it->second; }
      }
      if (auto it = argValues.find(name); it != argValues.end() && !it->second.isEmpty()) {
        val = it->second;
      } else if (auto itDef = ph->args.find("default"); itDef != ph->args.end()) {
        val = itDef->second;
      }
    }

    if (val.isEmpty() && ph->fallbackId.has_value()) {
      const auto &fb = *ph->fallbackId;
      if (fb == "clipboard") {
        if (auto *app = QCoreApplication::instance()) {
          if (auto *gui = qobject_cast<QGuiApplication *>(app)) {
            if (auto *clip = gui->clipboard()) val = clip->text();
          }
        }
      } else if (fb == "selected" || fb == "selection") {
        if (auto *reg = ServiceRegistry::instance()) {
          if (auto selService = reg->selectionService()) {
            if (auto selected = selService->selectedTextSync()) { val = *selected; }
          }
        }
      } else if (auto it = argValues.find(fb); it != argValues.end() && !it->second.isEmpty()) {
        val = it->second;
      } else {
        val = fb;
      }
    }

    bool isRaw = false;
    bool hasExplicitPercentEncode = false;

    for (const auto &mod : ph->modifiers) {
      if (mod.compare(u"raw", Qt::CaseInsensitive) == 0) {
        isRaw = true;
      } else if (mod.compare(u"percent-encode", Qt::CaseInsensitive) == 0 ||
                 mod.compare(u"url-encode", Qt::CaseInsensitive) == 0) {
        hasExplicitPercentEncode = true;
        val = QString::fromUtf8(QUrl::toPercentEncoding(val));
      } else {
        val = placeholder_utils::applyModifier(val, mod);
      }
    }

    if (isUrl && !isRaw && !hasExplicitPercentEncode &&
        (isArg || ph->id == "clipboard" || ph->id == "selected" || ph->id == "selection")) {
      val = QString::fromUtf8(QUrl::toPercentEncoding(val));
    }

    expanded += val;
  }

  return expanded;
}
