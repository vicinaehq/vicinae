#include "qml-theme-list-model.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/theme/theme-actions.hpp"
#include "clipboard-actions.hpp"
#include "keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"

QmlThemeListModel::QmlThemeListModel(QObject *parent) : QmlCommandListModel(parent) {}

void QmlThemeListModel::initialize(ApplicationContext *ctx) {
  QmlCommandListModel::initialize(ctx);
  m_themeService = &ThemeService::instance();
  m_config = ctx->services->config();
  m_previousThemeId = QString::fromStdString(m_config->value().systemTheme().name);

  connect(m_config, &config::Manager::configChanged, this,
          [this](const config::ConfigValue &next, const config::ConfigValue &prev) {
            if (next.systemTheme().name != prev.systemTheme().name) {
              regenerateThemes();
            }
          });
}

void QmlThemeListModel::setFilter(const QString &text) {
  m_query = text;
  auto themes = m_themeService->themes();

  m_selectedTheme.reset();
  m_availableThemes.clear();
  m_availableThemes.reserve(themes.size());

  auto currentId = QString::fromStdString(m_config->value().systemTheme().name);

  for (auto &theme : themes) {
    if (!theme->name().contains(text, Qt::CaseInsensitive)) continue;
    if (theme->id() == currentId) {
      m_selectedTheme = std::move(theme);
    } else {
      m_availableThemes.emplace_back(std::move(theme));
    }
  }

  std::vector<SectionInfo> sections;
  sections.push_back({QStringLiteral("Current Theme"), m_selectedTheme.has_value() ? 1 : 0});
  sections.push_back({QStringLiteral("Available Themes"), static_cast<int>(m_availableThemes.size())});
  setSections(sections);
}

void QmlThemeListModel::regenerateThemes() { setFilter(m_query); }

const std::shared_ptr<ThemeFile> &QmlThemeListModel::themeAt(int s, int i) const {
  if (s == 0) return *m_selectedTheme;
  return m_availableThemes[i];
}

QVariant QmlThemeListModel::data(const QModelIndex &index, int role) const {
  if (role < PaletteColor0) return QmlCommandListModel::data(index, role);

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

QHash<int, QByteArray> QmlThemeListModel::roleNames() const {
  auto roles = QmlCommandListModel::roleNames();
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

QString QmlThemeListModel::itemTitle(int s, int i) const { return themeAt(s, i)->name(); }

QString QmlThemeListModel::itemSubtitle(int s, int i) const {
  auto desc = themeAt(s, i)->description();
  return desc.isEmpty() ? QStringLiteral("Default theme description") : desc;
}

QString QmlThemeListModel::itemIconSource(int s, int i) const {
  const auto &theme = themeAt(s, i);
  if (theme->icon()) {
    return imageSourceFor(ImageURL::local(*theme->icon()).withFallback(ImageURL::builtin("vicinae")));
  }
  return imageSourceFor(ImageURL::builtin("vicinae"));
}

std::unique_ptr<ActionPanelState> QmlThemeListModel::createActionPanel(int s, int i) const {
  const auto &theme = themeAt(s, i);

  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  auto *setTheme = new SetThemeAction(theme->id());
  auto textEditor = ctx()->services->appDb()->textEditor();

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

void QmlThemeListModel::onItemSelected(int s, int i) {
  const auto &theme = themeAt(s, i);
  m_themeService->setTheme(theme->id());
}

void QmlThemeListModel::beforePop() {
  // Restore the configured theme when leaving the view
  m_themeService->setTheme(m_previousThemeId);
}
