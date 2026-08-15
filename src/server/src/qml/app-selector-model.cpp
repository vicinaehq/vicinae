#include "app-selector-model.hpp"
#include "view-utils.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

AppSelectorModel::AppSelectorModel(QObject *parent)
    : QObject(parent), m_appDb(ServiceRegistry::instance()->appDb()) {

  buildItems();
}

void AppSelectorModel::buildItems() {
  QVariantList allApps;

  if (auto browser = m_appDb->webBrowser()) {
    m_defaultEntry = qml::makeDropdownItem(QStringLiteral("default"),
                                           tr("%1 (Default)").arg(browser->fullyQualifiedName()),
                                           qml::imageSourceFor(browser->iconUrl()));
    allApps.append(m_defaultEntry);
  }

  for (const auto &app : m_appDb->list()) {
    if (!app->displayable()) continue;

    allApps.append(
        qml::makeDropdownItem(app->id(), app->fullyQualifiedName(), qml::imageSourceFor(app->iconUrl())));

    for (const auto &action : app->actions()) {
      allApps.append(qml::makeDropdownItem(action->id(), action->fullyQualifiedName(),
                                           qml::imageSourceFor(action->iconUrl())));
    }
  }

  m_model.setItems(allApps);
}

void AppSelectorModel::select(const QVariantMap &item) {
  m_currentItem = item;
  emit currentItemChanged();
}

void AppSelectorModel::selectById(const QString &id) {
  if (id == QStringLiteral("default") || id.isEmpty()) {
    if (!m_defaultEntry.isEmpty()) {
      m_currentItem = m_defaultEntry;
      emit currentItemChanged();
    }
    return;
  }

  if (auto item = m_model.itemDataById(id); !item.isEmpty()) {
    m_currentItem = item;
    emit currentItemChanged();
  }
}

void AppSelectorModel::updateDefaultApp(const std::shared_ptr<AbstractApplication> &app) {
  if (!app) return;

  m_defaultEntry =
      qml::makeDropdownItem(QStringLiteral("default"), tr("%1 (Default)").arg(app->fullyQualifiedName()),
                            qml::imageSourceFor(app->iconUrl()));
  m_model.updateItem(m_defaultEntry);

  if (m_currentItem[QStringLiteral("id")].toString() == QStringLiteral("default")) {
    m_currentItem = m_defaultEntry;
    emit currentItemChanged();
  }
}
