#include "actions/app/app-actions.hpp"
#include "clipboard-actions.hpp"
#include "common.hpp"
#include "keyboard/keybind.hpp"
#include "service-registry.hpp"
#include "services/config/config-service.hpp"
#include "theme.hpp"
#include "ui/typography/typography.hpp"
#include "actions/theme/theme-actions.hpp"
#include "ui/views/base-view.hpp"
#include "ui/views/typed-list-view.hpp"
#include <memory>
#include <qnamespace.h>
#include "manage-themes-view.hpp"

std::unique_ptr<ActionPanelState> ManageThemesView::createActionPanel(const ItemType &theme) const {
  auto panel = std::make_unique<ActionPanelState>();
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

  // we don't want to generate toml right now, we wait for action
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
  connect(config, &ConfigService::configChanged, this,
          [this](const ConfigService::Value &next, const ConfigService::Value &prev) {
            if (next.theme.name != prev.theme.name) { m_list->selectFirst(); }
          });
}

void ManageThemesView::initialize() {
  TypedListView::initialize();
  m_model = new ThemeListModel(context()->services->config(), &ThemeService::instance(), this);
  setModel(m_model);
  setSearchPlaceholderText("Search for a theme...");
}

void ManageThemesView::beforePop() {
  auto config = ServiceRegistry::instance()->config();
  auto &service = ThemeService::instance();
  service.setTheme(config->value().theme.name.value_or("vicinae-dark"));
}

void ManageThemesView::itemSelected(const ItemType &theme) {
  auto &service = ThemeService::instance();
  service.setTheme(theme->id());
}

void ManageThemesView::textChanged(const QString &s) { m_model->setFilter(s); }
