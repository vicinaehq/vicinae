#include "qml-empty-view-host.hpp"
#include "qml-utils.hpp"

QmlEmptyViewHost::QmlEmptyViewHost(const QString &title, const QString &description, const ImageURL &icon)
    : m_title(title), m_description(description) {
  if (icon.isValid()) m_iconSource = qml::imageSourceFor(icon);
}

QUrl QmlEmptyViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/EmptyView.qml"));
}

QVariantMap QmlEmptyViewHost::qmlProperties() const {
  QVariantMap props;
  props[QStringLiteral("title")] = m_title;
  props[QStringLiteral("description")] = m_description;
  if (!m_iconSource.isEmpty()) props[QStringLiteral("icon")] = m_iconSource;
  return props;
}
