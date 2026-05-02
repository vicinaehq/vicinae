#include "theme-view-host.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "fuzzy/scored.hpp"
#include "service-registry.hpp"
#include <algorithm>

void ThemeViewHost::initialize() {
  BaseView::initialize();

  m_themeService = &ThemeService::instance();
  m_config = context()->services->config();

  m_model.setScope(ViewScope(context(), this));
  m_model.addSource(&m_currentSection);
  m_model.addSource(&m_availableSection);

  m_currentSection.setOnThemeSelected(
      [this](const std::shared_ptr<ThemeFile> &theme) { m_themeService->setTheme(theme->id()); });
  m_availableSection.setOnThemeSelected(
      [this](const std::shared_ptr<ThemeFile> &theme) { m_themeService->setTheme(theme->id()); });

  setSearchPlaceholderText("Search for a theme...");

  connect(m_config, &config::Manager::configChanged, this,
          [this](const config::ConfigValue &next, const config::ConfigValue &prev) {
            if (next.systemTheme().name != prev.systemTheme().name) { regenerateThemes(); }
          });

  regenerateThemes();
}

void ThemeViewHost::textChanged(const QString &text) {
  m_query = text;
  regenerateThemes();
}

void ThemeViewHost::beforePop() {
  auto configuredTheme = QString::fromStdString(m_config->value().systemTheme().name);
  m_themeService->setTheme(configuredTheme);
}

void ThemeViewHost::regenerateThemes() {
  auto themes = m_themeService->themes();
  auto query = m_query.toStdString();
  auto currentId = QString::fromStdString(m_config->value().systemTheme().name);

  std::vector<std::shared_ptr<ThemeFile>> current;
  std::vector<Scored<std::shared_ptr<ThemeFile>>> scoredAvailable;

  for (auto &theme : themes) {
    int score = 0;
    if (!query.empty()) {
      auto name = theme->name().toStdString();
      auto desc = theme->description().toStdString();
      score = fuzzy::scoreWeighted({{name, 1.0}, {desc, 0.5}}, query);
      if (score == 0) continue;
    }

    if (theme->id() == currentId) {
      current.push_back(std::move(theme));
    } else {
      scoredAvailable.emplace_back(std::move(theme), score);
    }
  }

  if (!query.empty()) std::ranges::stable_sort(scoredAvailable, std::greater{});

  std::vector<std::shared_ptr<ThemeFile>> available;
  available.reserve(scoredAvailable.size());
  for (auto &s : scoredAvailable)
    available.emplace_back(std::move(s.data));

  m_currentSection.setThemes(QStringLiteral("Current Theme"), std::move(current));
  m_availableSection.setThemes(QStringLiteral("Available Themes"), std::move(available));
}
