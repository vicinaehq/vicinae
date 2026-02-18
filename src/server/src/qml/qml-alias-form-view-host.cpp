#include "qml-alias-form-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"
#include <QUrl>

QmlAliasFormViewHost::QmlAliasFormViewHost(EntrypointId id) : QmlFormViewBase(), m_id(std::move(id)) {}

QUrl QmlAliasFormViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/AliasFormView.qml"));
}

QVariantMap QmlAliasFormViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<QmlAliasFormViewHost *>(this))}};
}

void QmlAliasFormViewHost::initialize() {
  BaseView::initialize();

  auto manager = context()->services->rootItemManager();
  auto meta = manager->itemMetadata(m_id);

  m_alias = QString::fromStdString(meta.alias.value_or(""));
  emit formChanged();

  setNavigationTitle(QString("Set alias - %1").arg(meta.item->displayName()));
  setNavigationIcon(meta.item->iconUrl());

  auto panel = std::make_unique<FormActionPanelState>();
  auto section = panel->createSection();
  auto submitAction =
      new StaticAction(QStringLiteral("Submit"), ImageURL::builtin("enter-key"), [this]() { submit(); });
  section->addAction(submitAction);
  setActions(std::move(panel));
}

void QmlAliasFormViewHost::submit() {
  const auto toast = context()->services->toastService();
  const auto manager = context()->services->rootItemManager();

  m_aliasError.clear();
  emit errorsChanged();

  if (manager->setAlias(m_id, m_alias.toStdString())) {
    toast->setToast("Alias modified", ToastStyle::Success);
    popSelf();
  } else {
    toast->setToast("Failed to modify alias", ToastStyle::Danger);
  }
}
