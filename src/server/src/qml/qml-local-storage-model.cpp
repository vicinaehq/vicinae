#include "qml-local-storage-model.hpp"
#include "qml-local-storage-view-host.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"

QString QmlLocalStorageNamespaceModel::displayIconSource(const QString &) const {
  return imageSourceFor(ImageURL::builtin("coin"));
}

std::unique_ptr<ActionPanelState> QmlLocalStorageNamespaceModel::buildActionPanel(const QString &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  section->addAction(new StaticAction(
      "Browse namespace", ImageURL::builtin("coin"), [ns = item](ApplicationContext *ctx) {
        auto localStorage = ctx->services->localStorage();
        auto qkeys = localStorage->listNamespaceItems(ns).keys();
        std::vector<QString> keys(qkeys.begin(), qkeys.end());
        ctx->navigation->pushView(new QmlLocalStorageItemViewHost(ns, std::move(keys)));
      }));
  return panel;
}

QString QmlLocalStorageItemModel::displayIconSource(const QString &) const {
  return imageSourceFor(ImageURL::builtin("coin"));
}

std::unique_ptr<ActionPanelState> QmlLocalStorageItemModel::buildActionPanel(const QString &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  section->addAction(new StaticAction(
      "Show value", ImageURL::builtin("coin"), [ns = m_ns, key = item](ApplicationContext *ctx) {
        auto value = ctx->services->localStorage()->getItem(ns, key);
        ctx->services->toastService()->setToast(value.toString());
      }));
  return panel;
}
