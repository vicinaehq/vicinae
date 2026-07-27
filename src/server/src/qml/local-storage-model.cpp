#include "local-storage-model.hpp"
#include "local-storage-view-host.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"

std::optional<ImageURL> LocalStorageNamespaceSection::displayIcon(const QString &) const {
  return ImageURL::builtin(BuiltinIcon::Coin);
}

std::unique_ptr<ActionPanelState> LocalStorageNamespaceSection::buildActionPanel(const QString &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  section->addAction(new StaticAction(
      tr("Browse namespace"), ImageURL::builtin(BuiltinIcon::Coin), [ns = item](ApplicationContext *ctx) {
        auto localStorage = ctx->services->localStorage();
        auto qkeys = localStorage->listNamespaceItems(ns).keys();
        std::vector<QString> keys(qkeys.begin(), qkeys.end());
        ctx->navigation->pushView(new LocalStorageItemViewHost(ns, std::move(keys)));
      }));
  return panel;
}

std::optional<ImageURL> LocalStorageItemSection::displayIcon(const QString &) const {
  return ImageURL::builtin(BuiltinIcon::Coin);
}

std::unique_ptr<ActionPanelState> LocalStorageItemSection::buildActionPanel(const QString &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  section->addAction(new StaticAction(tr("Show value"), ImageURL::builtin(BuiltinIcon::Coin),
                                      [ns = m_ns, key = item](ApplicationContext *ctx) {
                                        auto value = ctx->services->localStorage()->getItem(ns, key);
                                        ctx->services->toastService()->setToast(value.toString());
                                      }));
  return panel;
}
