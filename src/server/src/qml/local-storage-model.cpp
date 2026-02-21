#include "local-storage-model.hpp"
#include "local-storage-view-host.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"

QString LocalStorageNamespaceModel::displayIconSource(const QString &) const {
  return imageSourceFor(ImageURL::builtin("coin"));
}

std::unique_ptr<ActionPanelState> LocalStorageNamespaceModel::buildActionPanel(const QString &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  section->addAction(new StaticAction(
      "Browse namespace", ImageURL::builtin("coin"), [ns = item](ApplicationContext *ctx) {
        auto localStorage = ctx->services->localStorage();
        auto qkeys = localStorage->listNamespaceItems(ns).keys();
        std::vector<QString> keys(qkeys.begin(), qkeys.end());
        ctx->navigation->pushView(new LocalStorageItemViewHost(ns, std::move(keys)));
      }));
  return panel;
}

QString LocalStorageItemModel::displayIconSource(const QString &) const {
  return imageSourceFor(ImageURL::builtin("coin"));
}

std::unique_ptr<ActionPanelState> LocalStorageItemModel::buildActionPanel(const QString &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  section->addAction(new StaticAction(
      "Show value", ImageURL::builtin("coin"), [ns = m_ns, key = item](ApplicationContext *ctx) {
        auto value = ctx->services->localStorage()->getItem(ns, key);
        ctx->services->toastService()->setToast(value.toString());
      }));
  return panel;
}
