#pragma once
#include "actions/theme/theme-actions.hpp"
#include "ui/views/base-view.hpp"
#include "../src/ui/image/url.hpp"
#include "ui/views/list-view.hpp"
#include "theme.hpp"
#include "ui/color-circle/color_circle.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/image/image.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "ui/typography/typography.hpp"
#include "utils/layout.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <qcolor.h>
#include <ranges>
#include <sys/socket.h>

class HorizontalColorPaletteWidget : public QWidget {
  std::vector<ColorCircle *> m_colors;
  ColorLike m_strokeColor = SemanticColor::Border;

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
    m_description->setColor(SemanticColor::TextSecondary);
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
  ThemeInfo m_theme;

public:
  bool hasUniformHeight() const override { return true; }

  QString generateId() const override { return m_theme.id; }

  bool recyclable() const override { return false; }

  void refresh(QWidget *widget) const override {
    auto item = static_cast<ThemeItemWidget *>(widget);

    item->setTitle(m_theme.name);
    item->setDescription(m_theme.description.isEmpty() ? "Default theme description" : m_theme.description);

    if (m_theme.icon) {
      item->setIcon(ImageURL::local(*m_theme.icon).withFallback(ImageURL::builtin("vicinae")));
    } else {
      item->setIcon(ImageURL::builtin("vicinae"));
    }

    std::vector<ColorLike> colors{
        m_theme.colors.base00, m_theme.colors.base01, m_theme.colors.base02, m_theme.colors.base03,
        m_theme.colors.base04, m_theme.colors.base05, m_theme.colors.base06, m_theme.colors.base07,
        m_theme.colors.base08, m_theme.colors.base09, m_theme.colors.base0A, m_theme.colors.base0B,
        m_theme.colors.base0C, m_theme.colors.base0D, m_theme.colors.base0E, m_theme.colors.base0F};

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
    auto setTheme = new SetThemeAction(m_theme.id);

    setTheme->setShortcut({.key = "return"});
    setTheme->setPrimary(true);

    panel->setTitle(m_theme.name);
    section->addAction(setTheme);

    return panel;
  }

  const ThemeInfo &theme() const { return m_theme; }

  ThemeItem(const ThemeInfo &theme) : m_theme(theme) {}
};

class SetThemeView : public ListView {
  void generateList(const QString &query) {
    auto &themeService = ThemeService::instance();

    m_list->updateModel([&]() {
      auto &current = themeService.theme();

      if (current.name.contains(query, Qt::CaseInsensitive)) {
        auto &section = m_list->addSection("Current Theme");

        section.addItem(std::make_unique<ThemeItem>(current));
      }

      if (!themeService.themes().empty()) {
        auto &section = m_list->addSection("Available Themes");

        auto filterTheme = [&](auto &&theme) {
          return theme.name != current.name && theme.name.contains(query, Qt::CaseInsensitive);
        };
        auto mapTheme = [](auto &&theme) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
          return std::make_unique<ThemeItem>(theme);
        };
        auto items = themeService.themes() | std::views::filter(filterTheme) |
                     std::views::transform(mapTheme) | std::ranges::to<std::vector>();

        section.addItems(std::move(items));
      }
    });
  }

  void initialize() override {
    textChanged("");
    setSearchPlaceholderText("Search for a theme...");
  }

  void textChanged(const QString &s) override { generateList(s); }

public:
  SetThemeView() {
    ThemeService::instance().scanThemeDirectories();
    connect(&ThemeService::instance(), &ThemeService::themeChanged, this,
            [this](const auto &info) { generateList(searchText()); });
  }
};
