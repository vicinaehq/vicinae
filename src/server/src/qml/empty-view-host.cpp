#include "empty-view-host.hpp"
#include "view-utils.hpp"

EmptyViewHost::EmptyViewHost(const QString &title, const QString &description, const ImageURL &icon)
    : m_title(title), m_description(description), m_icon(icon) {}

QUrl EmptyViewHost::qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/EmptyView.qml")); }

QVariantMap EmptyViewHost::qmlProperties() {
  QVariantMap props;
  props[QStringLiteral("title")] = m_title;
  props[QStringLiteral("description")] = m_description;
  if (m_icon.isValid()) props[QStringLiteral("icon")] = QVariant::fromValue(m_icon);
  return props;
}
