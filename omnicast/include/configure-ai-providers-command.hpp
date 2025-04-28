#include "app.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/declarative-omni-list-view.hpp"
#include "ui/default-list-item-widget.hpp"
#include "ui/omni-list.hpp"
#include "ui/toast.hpp"
#include <qnamespace.h>

using AIProvider = AI::Manager::ManagedProvider;

class ConfigureProviderAction : public AbstractAction {
  const AIProvider &m_info;

public:
  void execute(AppWindow &app) override {
    if (auto view = m_info.provider->configView(app)) {
      auto navTitle = QString("Configure %1 AI provider").arg(m_info.provider->displayName());

      app.pushView(
          view, {.navigation = NavigationStatus{
                     .title = navTitle,
                     .iconUrl =
                         BuiltinOmniIconUrl(m_info.provider->iconName()).setBackgroundTint(ColorTint::Red)}});
    } else {
      app.statusBar->setToast("This provider has no config view! This should not be the case.",
                              ToastPriority::Danger);
    }
  }

  ConfigureProviderAction(const AIProvider &info)
      : AbstractAction("Configure", BuiltinOmniIconUrl("cog")), m_info(info) {}
};

class ToggleAIProviderAction : public AbstractAction {
  const AIProvider &m_info;

  QString getActionText() { return m_info.enabled ? "Disable provider" : "Enable provider"; }

public:
  void execute(AppWindow &app) override {
    auto aiManager = ServiceRegistry::instance()->AI();
    bool success = aiManager->setProviderEnabled(m_info.provider->id(), !m_info.enabled);

    if (success) {
      app.statusBar->setToast("Provider status changed");
    } else {
      app.statusBar->setToast("Failed to change provider status");
    }
  }

  ToggleAIProviderAction(const AIProvider &info)
      : AbstractAction("Toggle status", BuiltinOmniIconUrl("stars")), m_info(info) {}
};

class AiProviderListItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
  const AIProvider &m_info;

  AccessoryList generateAccessories() const {
    AccessoryList accessories;

    accessories.emplace_back(ListAccessory{
        .text = m_info.configured ? "Configured" : "Not configured",
        .color = m_info.configured ? ColorTint::Blue : ColorTint::Orange,
        .fillBackground = true,
        .icon = BuiltinOmniIconUrl("cog"),
    });
    accessories.emplace_back(ListAccessory{
        .text = m_info.enabled ? "Enabled" : "Disabled",
        .color = m_info.enabled ? ColorTint::Green : ColorTint::Red,
        .fillBackground = true,
        .icon = BuiltinOmniIconUrl(m_info.enabled ? "checkmark" : "x-mark-circle"),
    });

    return accessories;
  }

public:
  std::vector<ActionItem> generateActionPannel() const override {
    std::vector<ActionItem> items;

    items.push_back(ActionLabel(m_info.provider->displayName()));
    items.push_back(std::make_unique<ConfigureProviderAction>(m_info));
    items.push_back(std::make_unique<ToggleAIProviderAction>(m_info));

    return items;
  }

  QString id() const override { return m_info.provider->id(); }
  ItemData data() const override {
    return {.iconUrl = BuiltinOmniIconUrl(m_info.provider->iconName()),
            .name = m_info.provider->displayName(),
            .accessories = generateAccessories()};
  }

  AiProviderListItem(const AIProvider &info) : m_info(info) {}
};

class ConfigureAIProvidersView : public DeclarativeOmniListView {
public:
  ItemList generateList(const QString &s) override {
    auto aiManager = ServiceRegistry::instance()->AI();
    ItemList list;

    list.reserve(aiManager->providers().size() + 2);
    list.emplace_back(std::make_unique<OmniList::VirtualSection>("Enabled"));

    for (const auto &provider : aiManager->providers()) {
      if (!(provider.configured && provider.enabled)) continue;
      if (!provider.provider->displayName().contains(s, Qt::CaseInsensitive)) { continue; }

      list.emplace_back(std::make_unique<AiProviderListItem>(provider));
    }

    list.emplace_back(std::make_unique<OmniList::VirtualSection>("Available"));

    for (const auto &provider : aiManager->providers()) {
      if (!provider.provider->displayName().contains(s, Qt::CaseInsensitive)) { continue; }
      if (!provider.enabled) { list.emplace_back(std::make_unique<AiProviderListItem>(provider)); }
    }

    return list;
  }

  void onMount() override { setSearchPlaceholderText("Search AI providers..."); }

  ConfigureAIProvidersView(AppWindow &app) : DeclarativeOmniListView(app) {}
};
