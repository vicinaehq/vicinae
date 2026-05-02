#include "theme-list-model.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/theme/theme-actions.hpp"
#include "clipboard-actions.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "fuzzy/scored.hpp"
#include "keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include <algorithm>
#include <utility>

void ThemeSection::setThemes(const QString &name, std::vector<std::shared_ptr<ThemeFile>> themes) {
  m_name = name;
  m_themes = std::move(themes);
  notifyChanged();
}

QString ThemeSection::itemTitle(int i) const { return m_themes[i]->name(); }

QString ThemeSection::itemSubtitle(int i) const {
  auto desc = m_themes[i]->description();
  return desc.isEmpty() ? QStringLiteral("Default theme description") : desc;
}

QString ThemeSection::itemIconSource(int i) const {
  const auto &theme = m_themes[i];
  if (theme->icon()) {
    return imageSourceFor(ImageURL::local(*theme->icon()).withFallback(ImageURL::builtin("vicinae")));
  }
  return imageSourceFor(ImageURL::builtin("vicinae"));
}

QVariant ThemeSection::customData(int i, int role) const {
  const auto &theme = m_themes[i];
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

QHash<int, QByteArray> ThemeSection::customRoleNames() const {
  return {
      {PaletteColor0, "paletteColor0"}, {PaletteColor1, "paletteColor1"}, {PaletteColor2, "paletteColor2"},
      {PaletteColor3, "paletteColor3"}, {PaletteColor4, "paletteColor4"}, {PaletteColor5, "paletteColor5"},
      {PaletteColor6, "paletteColor6"}, {PaletteColor7, "paletteColor7"},
  };
}

std::unique_ptr<ActionPanelState> ThemeSection::actionPanel(int i) const {
  const auto &theme = m_themes[i];

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
