#pragma once
#include "builtin_icon.hpp"
#include "common/context.hpp"
#include "fuzzy-section.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include <QCoreApplication>
#include <ranges>

struct WorkspaceInfo {
  std::optional<AbstractWindowManager::Screen> screen;
  AbstractWindowManager::WorkspacePtr workspace;
  size_t windowCount = 0;
  std::vector<std::shared_ptr<AbstractApplication>> apps;
};

template <> struct fuzzy::FuzzySearchable<WorkspaceInfo> {
  static fuzzy::Match score(const WorkspaceInfo &e, const fuzzy::Query &query) {
    std::vector<OwnedWeightedField> fields{};

    fields.push_back({e.workspace->name().toStdString(), 1.0});

    if (auto s = e.screen) { fields.push_back({e.screen->name.toStdString(), 0.8}); }

    for (const auto &app : e.apps) {
      fields.push_back({app->displayName().toStdString(), 0.3});
    }

    return fuzzy::scoreWeighted(fields, query);
  }
};

class SwitchWorkspacesSection : public FuzzySection<WorkspaceInfo> {
  Q_DECLARE_TR_FUNCTIONS(SwitchWorkspacesSection)

public:
  SwitchWorkspacesSection(QString label) : m_label(std::move(label)) {}

  QString sectionName() const override { return m_label; }

protected:
  QString displayTitle(const WorkspaceInfo &e) const override { return tr("%1").arg(e.workspace->name()); }

  QString displaySubtitle(const WorkspaceInfo &e) const override {
    QString strs = e.windowCount ? tr("%n window(s)", "", e.windowCount) : tr("empty");

    if (e.screen) { strs += " - " + e.screen->name; }

    return strs;
  }

  AccessoryList displayAccessories(const WorkspaceInfo &e) const override {
    return e.apps | std::views::transform([](auto &&app) {
             return ListAccessory{.tooltip = app->displayName(), .icon = app->iconUrl()};
           }) |
           std::ranges::to<std::vector>();
  }

  std::optional<ImageURL> displayIcon(const WorkspaceInfo &e) const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowGrid3x3);
  }

  std::unique_ptr<ActionPanelState> buildActionPanel(const WorkspaceInfo &e) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto main = panel->createSection();
    auto focus = new StaticAction(tr("Switch to workspace"), ImageURL::builtin(BuiltinIcon::SwitchWindows),
                                  [w = e.workspace](ApplicationContext *ctx) {
                                    ctx->services->windowManager()->provider()->focusWorkspaceSync(*w);
                                    ctx->navigation->closeWindow();
                                  });

    main->addAction(focus);

    return panel;
  };

private:
  QString m_label;
};
