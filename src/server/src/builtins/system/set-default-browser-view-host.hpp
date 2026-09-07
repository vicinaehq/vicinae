#pragma once
#include <QCoreApplication>
#include <memory>
#include <optional>
#include <ranges>
#include "services/builtin-icon/builtin-icon.hpp"
#include "common/context.hpp"
#include "ui/views/mono-list-view-host.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/views/list-accessory.hpp"

class SetDefaultBrowserViewHost : public MonoListViewHost<std::shared_ptr<AbstractApplication>> {
  Q_DECLARE_TR_FUNCTIONS(SetDefaultBrowserViewHost)

public:
  void onMount() override {
    setSearchPlaceholderText(tr("Select a web browser..."));
    reloadItems();
  }

  QString sectionName() const override { return tr("Available web browsers"); }

  QString displayTitle(const ItemType &e) const override { return e->displayName(); }

  QString displaySubtitle(const ItemType &e) const override { return e->description(); }

  std::optional<ImageURL> displayIcon(const ItemType &e) const override { return e->iconUrl(); }

  AccessoryList displayAccessories(const ItemType &e) const override {
    if (m_defaultBrowser && *m_defaultBrowser == e->id()) {
      return {
          ListAccessory{.icon = ImageURL::builtin(BuiltinIcon::CheckCircle).setFill(SemanticColor::Green)}};
    }

    return {};
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &e) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto main = panel->createSection();
    auto setDefault = new StaticAction(
        tr("Set as default browser"), ImageURL::builtin(BuiltinIcon::Globe01),
        [app = e](ApplicationContext *ctx) {
          if (ctx->services->appDb()->setWebBrowser(*app)) {
            ctx->navigation->showHud(tr("Default browser changed"),
                                     ImageURL::builtin(BuiltinIcon::Globe01).setFill(SemanticColor::Green));
            ctx->navigation->popToRoot();
          } else {
            ctx->services->toastService()->failure(tr("Failed to set default browser"));
          }
        });

    main->addAction(setDefault);

    return panel;
  }

private:
  static constexpr auto PROBE_URL = "https://vicinae.com";

  void reloadItems() {
    auto appDb = context()->services->appDb();

    if (auto app = appDb->webBrowser()) { m_defaultBrowser = app->id(); }

    auto browsers = appDb->findOpeners(PROBE_URL) |
                    std::views::filter([](auto &&app) { return app->displayable(); }) |
                    std::ranges::to<std::vector>();
    const auto isDefault = [&](auto &&app) { return m_defaultBrowser && *m_defaultBrowser == app->id(); };

    std::ranges::stable_sort(browsers, [&](auto &&a, auto &&b) { return isDefault(a) > isDefault(b); });

    setItems(std::move(browsers));
    notifyChanged();
  }

  std::optional<QString> m_defaultBrowser;
};
