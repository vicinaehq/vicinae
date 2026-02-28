#include "theme-list-model.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/theme/theme-actions.hpp"
#include "clipboard-actions.hpp"
#include "common/scored.hpp"
#include "keyboard/keybind.hpp"
#include "lib/fts_fuzzy.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include <algorithm>

ThemeListModel::ThemeListModel(QObject *parent) : CommandListModel(parent) {}

void ThemeListModel::initialize() {
  m_themeService = &ThemeService::instance();
  m_config = scope().services()->config();
  connect(m_config, &config::Manager::configChanged, this,
          [this](const config::ConfigValue &next, const config::ConfigValue &prev) {
            if (next.systemTheme().name != prev.systemTheme().name) { regenerateThemes(); }
          });
}

void ThemeListModel::setFilter(const QString &text) {
  m_query = text;
  auto themes = m_themeService->themes();
  auto query = text.toStdString();

  m_selectedTheme.reset();
  m_availableThemes.clear();

  auto currentId = QString::fromStdString(m_config->value().systemTheme().name);
  std::vector<Scored<std::shared_ptr<ThemeFile>>> scoredAvailable;

  for (auto &theme : themes) {
    int score = 0;
    if (!query.empty()) {
      auto name = theme->name().toStdString();
      if (!fts::fuzzy_match(query, name, score)) continue;
    }

    if (theme->id() == currentId) {
      m_selectedTheme = std::move(theme);
    } else {
      scoredAvailable.emplace_back(std::move(theme), score);
    }
  }

  if (!query.empty()) std::ranges::stable_sort(scoredAvailable, std::greater{});

  m_availableThemes.reserve(scoredAvailable.size());
  for (auto &s : scoredAvailable)
    m_availableThemes.emplace_back(std::move(s.data));

  std::vector<SectionInfo> sections;
  sections.emplace_back(QStringLiteral("Current Theme"), m_selectedTheme.has_value() ? 1 : 0);
  sections.emplace_back(QStringLiteral("Available Themes"), static_cast<int>(m_availableThemes.size()));
  setSections(sections);
}

void ThemeListModel::regenerateThemes() { setFilter(m_query); }

const std::shared_ptr<ThemeFile> &ThemeListModel::themeAt(int s, int i) const {
  if (s == 0 && m_selectedTheme.has_value()) return *m_selectedTheme;
  return m_availableThemes[i];
}

QVariant ThemeListModel::data(const QModelIndex &index, int role) const {
  if (role < PaletteColor0) return CommandListModel::data(index, role);

  int s, i;
  if (!dataItemAt(index.row(), s, i)) return {};

  const auto &theme = themeAt(s, i);

  switch (role) {
  case PaletteColor0:
    return theme->resolve(SemanticColor::Red);
  case PaletteColor1:
    return theme->resolve(SemanticColor::Blue);
  case PaletteColor2:
    return theme->resolve(SemanticColor::Cyan);
  case PaletteColor3:
    return theme->resolve(SemanticColor::Green);
  case PaletteColor4:
    return theme->resolve(SemanticColor::Magenta);
  case PaletteColor5:
    return theme->resolve(SemanticColor::Orange);
  case PaletteColor6:
    return theme->resolve(SemanticColor::Foreground);
  case PaletteColor7:
    return theme->resolve(SemanticColor::TextMuted);
  default:
    return {};
  }
}

QHash<int, QByteArray> ThemeListModel::roleNames() const {
  auto roles = CommandListModel::roleNames();
  roles[PaletteColor0] = "paletteColor0";
  roles[PaletteColor1] = "paletteColor1";
  roles[PaletteColor2] = "paletteColor2";
  roles[PaletteColor3] = "paletteColor3";
  roles[PaletteColor4] = "paletteColor4";
  roles[PaletteColor5] = "paletteColor5";
  roles[PaletteColor6] = "paletteColor6";
  roles[PaletteColor7] = "paletteColor7";
  return roles;
}

QString ThemeListModel::itemTitle(int s, int i) const { return themeAt(s, i)->name(); }

QString ThemeListModel::itemSubtitle(int s, int i) const {
  auto desc = themeAt(s, i)->description();
  return desc.isEmpty() ? QStringLiteral("Default theme description") : desc;
}

QString ThemeListModel::itemIconSource(int s, int i) const {
  const auto &theme = themeAt(s, i);
  if (theme->icon()) {
    return imageSourceFor(ImageURL::local(*theme->icon()).withFallback(ImageURL::builtin("vicinae")));
  }
  return imageSourceFor(ImageURL::builtin("vicinae"));
}

std::unique_ptr<ActionPanelState> ThemeListModel::createActionPanel(int s, int i) const {
  const auto &theme = themeAt(s, i);

  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  auto *setTheme = new SetThemeAction(theme->id());
  auto textEditor = scope().services()->appDb()->textEditor();

  panel->setTitle(theme->name());
  section->addAction(setTheme);

  if (theme->path() && textEditor) {
    auto *open = new OpenAppAction(textEditor, "Open theme file", {theme->path()->c_str()});
    open->setShortcut(Keybind::OpenAction);
    section->addAction(open);
  }

  auto *utils = panel->createSection();
  auto *copyId = new CopyToClipboardAction(Clipboard::Text(theme->id()), "Copy ID");
  copyId->setShortcut(Keybind::CopyNameAction);
  utils->addAction(copyId);

  if (theme->path()) {
    auto *copyPath = new CopyToClipboardAction(Clipboard::Text(theme->path()->c_str()), "Copy path");
    copyPath->setShortcut(Keybind::CopyPathAction);
    utils->addAction(copyPath);
  }

  return panel;
}

void ThemeListModel::onItemSelected(int s, int i) {
  const auto &theme = themeAt(s, i);
  m_themeService->setTheme(theme->id());
}

void ThemeListModel::beforePop() {
  auto configuredTheme = QString::fromStdString(m_config->value().systemTheme().name);
  m_themeService->setTheme(configuredTheme);
}
