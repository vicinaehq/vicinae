#include "qml-app-selector-model.hpp"
#include "qml-utils.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

QmlAppSelectorModel::QmlAppSelectorModel(QObject *parent) : QObject(parent) {
  m_appDb = ServiceRegistry::instance()->appDb();
  buildItems();
}

void QmlAppSelectorModel::buildItems() {
  m_items.clear();

  QVariantList allApps;

  if (auto browser = m_appDb->webBrowser()) {
    m_defaultEntry = QVariantMap{
        {QStringLiteral("id"), QStringLiteral("default")},
        {QStringLiteral("displayName"), browser->fullyQualifiedName() + " (Default)"},
        {QStringLiteral("iconSource"), qml::imageSourceFor(browser->iconUrl())},
    };
    allApps.append(m_defaultEntry);
  }

  for (const auto &app : m_appDb->list()) {
    if (!app->displayable()) continue;

    allApps.append(QVariantMap{
        {QStringLiteral("id"), app->id()},
        {QStringLiteral("displayName"), app->fullyQualifiedName()},
        {QStringLiteral("iconSource"), qml::imageSourceFor(app->iconUrl())},
    });

    for (const auto &action : app->actions()) {
      allApps.append(QVariantMap{
          {QStringLiteral("id"), action->id()},
          {QStringLiteral("displayName"), action->fullyQualifiedName()},
          {QStringLiteral("iconSource"), qml::imageSourceFor(action->iconUrl())},
      });
    }
  }

  QVariantMap section;
  section[QStringLiteral("title")] = QString();
  section[QStringLiteral("items")] = allApps;
  m_items.append(section);

  emit itemsChanged();
}

void QmlAppSelectorModel::select(const QVariantMap &item) {
  m_currentItem = item;
  emit currentItemChanged();
}

void QmlAppSelectorModel::selectById(const QString &id) {
  if (id == QStringLiteral("default") || id.isEmpty()) {
    if (!m_defaultEntry.isEmpty()) {
      m_currentItem = m_defaultEntry;
      emit currentItemChanged();
    }
    return;
  }

  for (const auto &sectionVar : m_items) {
    auto section = sectionVar.toMap();
    auto items = section[QStringLiteral("items")].toList();
    for (const auto &itemVar : items) {
      auto item = itemVar.toMap();
      if (item[QStringLiteral("id")].toString() == id) {
        m_currentItem = item;
        emit currentItemChanged();
        return;
      }
    }
  }
}

void QmlAppSelectorModel::updateDefaultApp(const std::shared_ptr<AbstractApplication> &app) {
  if (!app) return;

  m_defaultEntry = QVariantMap{
      {QStringLiteral("id"), QStringLiteral("default")},
      {QStringLiteral("displayName"), app->fullyQualifiedName() + " (Default)"},
      {QStringLiteral("iconSource"), qml::imageSourceFor(app->iconUrl())},
  };

  if (!m_items.isEmpty()) {
    auto section = m_items[0].toMap();
    auto items = section[QStringLiteral("items")].toList();
    if (!items.isEmpty()) {
      auto first = items[0].toMap();
      if (first[QStringLiteral("id")].toString() == QStringLiteral("default")) {
        items[0] = m_defaultEntry;
        section[QStringLiteral("items")] = items;
        m_items[0] = section;
        emit itemsChanged();
      }
    }
  }

  if (m_currentItem[QStringLiteral("id")].toString() == QStringLiteral("default")) {
    m_currentItem = m_defaultEntry;
    emit currentItemChanged();
  }
}
