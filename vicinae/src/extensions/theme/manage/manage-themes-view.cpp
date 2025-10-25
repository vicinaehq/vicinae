#include "actions/app/app-actions.hpp"
#include "clipboard-actions.hpp"
#include "common.hpp"
#include "keyboard/keybind.hpp"
#include "service-registry.hpp"
#include "services/config/config-service.hpp"
#include "theme.hpp"
#include "ui/color-circle/color_circle.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/image/image.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "ui/typography/typography.hpp"
#include "utils/layout.hpp"
#include "actions/theme/theme-actions.hpp"
#include "theme/colors.hpp"
#include "ui/views/base-view.hpp"
#include "ui/image/url.hpp"

#include <memory>
#include <qnamespace.h>
#include "manage-themes-view.hpp"

class HorizontalColorPaletteWidget : public QWidget {
  std::vector<ColorCircle *> m_colors;
  QColor m_strokeColor = "#CCCCCC";

public:
  HorizontalColorPaletteWidget() {}

  void setColors(const std::vector<ColorLike> &colors) {
    HStack()
        .spacing(3)
        .map(colors,
             [&](const ColorLike &color) {
               auto circle = new ColorCircle({16, 16});
               circle->setColor(color);
               circle->setStroke(SemanticColor::TextPrimary, 2);
               return circle;
             })
        .imbue(this);
  }
};

class ThemeItemWidget : public SelectableOmniListWidget {
  ImageWidget *m_icon = new ImageWidget();
  TypographyWidget *m_title = new TypographyWidget();
  TypographyWidget *m_description = new TypographyWidget();
  AccessoryListWidget *m_accessories = new AccessoryListWidget(this);
  QWidget *m_textWidget = new QWidget(this);
  HorizontalColorPaletteWidget *m_palette = new HorizontalColorPaletteWidget();

public:
  void setIcon(const ImageURL &url) { m_icon->setUrl(url); }
  void setTitle(const QString &title) { m_title->setText(title); }
  void setDescription(const QString &description) { m_description->setText(description); }
  void setColors(const std::vector<ColorLike> &colors) { m_palette->setColors(colors); }

  ThemeItemWidget(QWidget *parent = nullptr) : SelectableOmniListWidget(parent) {
    m_description->setColor(SemanticColor::TextMuted);
    m_icon->setFixedSize(30, 30);

    HStack()
        .margins(10)
        .justifyBetween()
        .spacing(10)
        .add(HStack().spacing(10).add(m_icon).add(VStack().spacing(2).add(m_title).add(m_description)))
        .add(m_palette)
        .imbue(this);
  }
};

class ThemeItem : public OmniList::AbstractVirtualItem, public ListView::Actionnable {

public:
  bool hasUniformHeight() const override { return true; }

  QString generateId() const override { return m_theme->id(); }

  bool recyclable() const override { return false; }

  void refresh(QWidget *widget) const override {
    auto item = static_cast<ThemeItemWidget *>(widget);

    item->setTitle(m_theme->name());
    item->setDescription(m_theme->description().isEmpty() ? "Default theme description"
                                                          : m_theme->description());

    if (m_theme->icon()) {
      item->setIcon(ImageURL::local(*m_theme->icon()).withFallback(ImageURL::builtin("vicinae")));
    } else {
      item->setIcon(ImageURL::builtin("vicinae"));
    }

    std::vector<ColorLike> colors{
        m_theme->resolve(SemanticColor::Red),        m_theme->resolve(SemanticColor::Blue),
        m_theme->resolve(SemanticColor::Cyan),       m_theme->resolve(SemanticColor::Green),
        m_theme->resolve(SemanticColor::Magenta),    m_theme->resolve(SemanticColor::Orange),
        m_theme->resolve(SemanticColor::Foreground), m_theme->resolve(SemanticColor::TextMuted)};

    item->setColors(colors);
  }

  OmniListItemWidget *createWidget() const override {
    auto item = new ThemeItemWidget;
    refresh(item);
    return item;
  }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();
    auto setTheme = new SetThemeAction(m_theme->id());
    auto textEditor = ctx->services->appDb()->textEditor();

    panel->setTitle(m_theme->name());
    section->addAction(setTheme);

    if (m_theme->path() && textEditor) {
      auto open = new OpenAppAction(textEditor, "Open theme file", {m_theme->path()->c_str()});
      open->setShortcut(Keybind::OpenAction);
      section->addAction(open);
    }

    auto utils = panel->createSection();

    // we don't want to generate toml right now, we wait for action
    auto copyId = new CopyToClipboardAction(Clipboard::Text(m_theme->id()), "Copy ID");

    copyId->setShortcut(Keybind::CopyNameAction);
    utils->addAction(copyId);

    if (m_theme->path()) {
      auto copyPath = new CopyToClipboardAction(Clipboard::Text(m_theme->path()->c_str()), "Copy path");
      copyPath->setShortcut(Keybind::CopyPathAction);
      utils->addAction(copyPath);
    }

    return panel;
  }

  const ThemeFile &theme() const { return *m_theme; }

  ThemeItem(const ThemeFile &theme) : m_theme(std::make_shared<ThemeFile>(theme)) {}

  std::shared_ptr<ThemeFile> m_theme;
};

ManageThemesView::ManageThemesView() {
  auto config = ServiceRegistry::instance()->config();
  connect(config, &ConfigService::configChanged, this,
          [this](const ConfigService::Value &next, const ConfigService::Value &prev) {
            if (next.theme.name != prev.theme.name) { generateList(searchText()); }
          });
}

void ManageThemesView::initialize() {
  textChanged("");
  setSearchPlaceholderText("Search for a theme...");
}

void ManageThemesView::beforePop() {
  auto config = ServiceRegistry::instance()->config();
  auto &service = ThemeService::instance();
  service.setTheme(config->value().theme.name.value_or("vicinae-dark"));
}

void ManageThemesView::itemSelected(const OmniList::AbstractVirtualItem *item) {
  auto &service = ThemeService::instance();
  auto themeItem = static_cast<const ThemeItem *>(item);
  service.setTheme(themeItem->theme());
}

void ManageThemesView::textChanged(const QString &s) { generateList(s); }

void ManageThemesView::generateList(const QString &query) {
  auto &themeService = ThemeService::instance();
  auto config = ServiceRegistry::instance()->config();
  auto currentThemeName = config->value().theme.name.value_or("vicinae-dark");

  m_list->updateModel([&]() {
    if (currentThemeName.contains(query, Qt::CaseInsensitive)) {
      auto &section = m_list->addSection("Current Theme");
      if (auto theme = themeService.findTheme(currentThemeName)) {
        section.addItem(std::make_unique<ThemeItem>(*theme));
      }
    }

    if (!themeService.themes().empty()) {
      auto &section = m_list->addSection("Available Themes");

      for (const auto &theme : themeService.themes()) {
        bool filtered = theme->id() != currentThemeName && theme->name().contains(query, Qt::CaseInsensitive);
        if (filtered) { section.addItem(std::make_unique<ThemeItem>(*theme)); }
      }
    }
  });
}
