#pragma once
#include "app.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/action_popover.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include <memory>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <sys/socket.h>

class SetThemeAction : public AbstractAction {
  QString _themeName;

  void execute(AppWindow &app) override {
    ThemeService::instance().setTheme(_themeName);
    app.statusBar->setToast("Theme set to " + _themeName);
  }

public:
  SetThemeAction(const QString &themeName)
      : AbstractAction("Set theme", BuiltinOmniIconUrl("brush")), _themeName(themeName) {}
};

class ThemeItem : public AbstractDefaultListItem, public OmniListView::IActionnable {
  const ThemeInfo &info;
  std::function<void(const QString &name)> _themeSelectedCallback;

public:
  QList<AbstractAction *> generateActions() const override {
    auto set = new SetThemeAction(info.name);

    if (_themeSelectedCallback) {
      set->setExecutionCallback([this]() { _themeSelectedCallback(info.name); });
    }

    return {set};
  }

  const QString &name() const { return info.name; }

  void setSelectedCallback(const std::function<void(const QString &name)> &cb) {
    _themeSelectedCallback = cb;
  }

  ItemData data() const override {
    return {
        .iconUrl = BuiltinOmniIconUrl("brush"),
        .name = info.name,
    };
  }

  QString id() const override { return info.name; }

public:
  ThemeItem(const ThemeInfo &info) : info(info) {}
};

class ManageThemesView : public OmniListView {
  void generateList(const QString &query) {
    auto &themeService = ThemeService::instance();
    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> items;
    auto &current = themeService.theme();

    list->clearSelection();

    if (current.name.contains(query, Qt::CaseInsensitive)) {
      items.push_back(std::make_unique<OmniList::VirtualSection>("Current Theme"));
      items.push_back(std::make_unique<ThemeItem>(current));
    }

    items.push_back(std::make_unique<OmniList::VirtualSection>("Available Themes"));

    for (const auto &theme : themeService.themes()) {
      if (theme.name == current.name || !theme.name.contains(query, Qt::CaseInsensitive)) continue;
      auto candidate = std::make_unique<ThemeItem>(theme);

      candidate->setSelectedCallback([this, query](const QString &name) {
        list->clearSelection();
        generateList(query);
      });

      items.push_back(std::move(candidate));
    }

    list->updateFromList(items, OmniList::SelectionPolicy::SelectFirst);
  }

  void onMount() override { setSearchPlaceholderText("Manage themes..."); }

  void onSearchChanged(const QString &s) override { generateList(s); }

public:
  ManageThemesView(AppWindow &app) : OmniListView(app) {}
};
