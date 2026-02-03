#include "actions/app/app-actions.hpp"
#include "clipboard-actions.hpp"
#include "common.hpp"
#include "keyboard/keybind.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/typography/typography.hpp"
#include "actions/theme/theme-actions.hpp"
#include "ui/views/base-view.hpp"
#include "ui/views/typed-list-view.hpp"
#include "config/config.hpp"
#include <memory>
#include "manage-themes-view.hpp"

std::unique_ptr<ActionPanelState> ManageThemesView::createActionPanel(const ItemType &theme) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();
  auto setTheme = new SetThemeAction(theme->id());
  auto textEditor = context()->services->appDb()->textEditor();

  panel->setTitle(theme->name());
  section->addAction(setTheme);

  if (theme->path() && textEditor) {
    auto open = new OpenAppAction(textEditor, "Open theme file", {theme->path()->c_str()});
    open->setShortcut(Keybind::OpenAction);
    section->addAction(open);
  }

  auto utils = panel->createSection();
  auto copyId = new CopyToClipboardAction(Clipboard::Text(theme->id()), "Copy ID");

  copyId->setShortcut(Keybind::CopyNameAction);
  utils->addAction(copyId);

  if (theme->path()) {
    auto copyPath = new CopyToClipboardAction(Clipboard::Text(theme->path()->c_str()), "Copy path");
    copyPath->setShortcut(Keybind::CopyPathAction);
    utils->addAction(copyPath);
  }

  return panel;
}

ManageThemesView::ManageThemesView() {
  auto config = ServiceRegistry::instance()->config();
  connect(config, &config::Manager::configChanged, this,
          [this](const config::ConfigValue &next, const config::ConfigValue &prev) {
            if (next.systemTheme().name != prev.systemTheme().name) { m_list->selectFirst(); }
          });
}

void ManageThemesView::initialize() {
  TypedListView::initialize();
  m_model = new ThemeListModel(this);
  m_controller =
      new ThemeListController(context()->services->config(), &ThemeService::instance(), m_model, this);
  setModel(m_model);
  setSearchPlaceholderText("Search for a theme...");
}

void ManageThemesView::beforePop() {
  auto config = ServiceRegistry::instance()->config();
  auto &service = ThemeService::instance();
  service.setTheme(config->value().systemTheme().name.c_str());
}

void ManageThemesView::itemSelected(const ItemType &theme) {
  auto &service = ThemeService::instance();
  service.setTheme(theme->id());
}

void ManageThemesView::textChanged(const QString &s) { m_controller->setFilter(s); }
