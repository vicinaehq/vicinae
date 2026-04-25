#include "theme-list-controller.hpp"
#include "config/config.hpp"
#include "lib/fuzzy/fuzzy-searchable.hpp"
#include "theme-list-model.hpp"

ThemeListController::ThemeListController(config::Manager *configService, ThemeService *themeService,
                                         ThemeListModel *model, QObject *parent)
    : QObject(parent), m_model(model), m_configService(configService), m_themeService(themeService) {

  connect(m_configService, &config::Manager::configChanged, this, &ThemeListController::handleConfigChanged);
  regenerateThemes();
}

void ThemeListController::setFilter(const QString &query) {
  m_query = query;
  auto themes = m_themeService->themes();

  std::optional<std::shared_ptr<ThemeFile>> selectedTheme;
  std::vector<std::shared_ptr<ThemeFile>> availableThemes;
  availableThemes.reserve(themes.size() - 1);

  std::string const q = query.toStdString();
  for (auto &theme : themes) {
    if (!q.empty()) {
      auto name = theme->name().toStdString();
      auto desc = theme->description().toStdString();
      if (fuzzy::scoreWeighted({{name, 1.0}, {desc, 0.5}}, q) == 0) continue;
    }
    if (theme->id() == m_configService->value().systemTheme().name) {
      selectedTheme = std::move(theme);
    } else {
      availableThemes.emplace_back(std::move(theme));
    }
  }

  m_model->setThemes(selectedTheme, availableThemes);
}

void ThemeListController::regenerateThemes() { setFilter(m_query); }

void ThemeListController::handleConfigChanged() { regenerateThemes(); }
