#include "empty-view-host.hpp"
#include "view-utils.hpp"

EmptyViewHost::EmptyViewHost(const QString &title, const QString &description, const ImageURL &icon)
    : m_title(title), m_description(description) {
  if (icon.isValid()) m_iconSource = qml::imageSourceFor(icon);
}

QUrl EmptyViewHost::qmlComponentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/EmptyView.qml")); }

QVariantMap EmptyViewHost::qmlProperties() const {
  QVariantMap props;
  props[QStringLiteral("title")] = m_title;
  props[QStringLiteral("description")] = m_description;
  if (!m_iconSource.isEmpty()) props[QStringLiteral("icon")] = m_iconSource;
  return props;
}
