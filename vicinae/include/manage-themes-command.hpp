#pragma once
#include "action-panel/action-panel.hpp"
#include "actions/theme/theme-actions.hpp"
#include "ui/views/base-view.hpp"
#include "../src/ui/image/url.hpp"
#include "service-registry.hpp"
#include "ui/views/list-view.hpp"
#include "theme.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/color-circle/color_circle.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "ui/typography/typography.hpp"
#include <algorithm>
#include <memory>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <ranges>
#include <sys/socket.h>

class HorizontalColorPaletteWidget : public QWidget {
  QHBoxLayout *m_layout = new QHBoxLayout;
  std::vector<ColorCircle *> m_colors;
  QColor m_strokeColor = "#CCCCCC";

public:
  HorizontalColorPaletteWidget() {
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
  }

  void setStrokeColor(const QColor &color) { m_strokeColor = color; }

  void setColors(const std::vector<ColorLike> &colors) {
    while (m_layout->count() > 0) {
      m_layout->takeAt(0)->widget()->deleteLater();
    }

    std::ranges::for_each(colors, [&](const ColorLike &color) {
      auto circle = new ColorCircle({16, 16});

      circle->setColor(color);
      circle->setStroke(m_strokeColor, 2);
      m_layout->addWidget(circle);
    });
  }
};

class ThemeItemWidget : public SelectableOmniListWidget {
  QHBoxLayout *m_layout = new QHBoxLayout(this);
  Omnimg::ImageWidget *m_icon = new Omnimg::ImageWidget();
  TypographyWidget *m_title = new TypographyWidget();
  TypographyWidget *m_description = new TypographyWidget();
  AccessoryListWidget *m_accessories = new AccessoryListWidget(this);
  QWidget *m_textWidget = new QWidget(this);
  QVBoxLayout *m_textLayout = new QVBoxLayout(m_textWidget);
  HorizontalColorPaletteWidget *m_palette = new HorizontalColorPaletteWidget();

public:
  void setIcon(const ImageURL &url) { m_icon->setUrl(url); }
  void setTitle(const QString &title) { m_title->setText(title); }
  void setDescription(const QString &description) { m_description->setText(description); }
  void setColors(const std::vector<ColorLike> &colors) { m_palette->setColors(colors); }
  void setStrokeColor(const QColor &color) { m_palette->setStrokeColor(color); }

  ThemeItemWidget(QWidget *parent = nullptr) : SelectableOmniListWidget(parent) {
    m_description->setColor(SemanticColor::TextSecondary);
    m_textLayout->addWidget(m_title);
    m_textLayout->setContentsMargins(0, 0, 0, 0);
    m_textLayout->setSpacing(2);
    m_textLayout->addWidget(m_description);
    m_textWidget->setLayout(m_textLayout);

    m_icon->setFixedSize(30, 30);

    m_layout->setSpacing(10);
    m_layout->addWidget(m_icon);
    m_layout->addWidget(m_textWidget);
    m_layout->addWidget(m_palette, 0, Qt::AlignRight);
    m_layout->setContentsMargins(10, 10, 10, 10);

    setLayout(m_layout);
  }
};

class ThemeItem : public OmniList::AbstractVirtualItem, public ListView::Actionnable {
  ThemeInfo m_theme;

public:
  bool hasUniformHeight() const override { return true; }

  QString generateId() const override { return m_theme.id; }

  bool recyclable() const override { return false; }

  OmniListItemWidget *createWidget() const override {
    auto item = new ThemeItemWidget;

    item->setTitle(m_theme.name);
    item->setDescription(m_theme.description.isEmpty() ? "Default theme description" : m_theme.description);

    if (m_theme.icon) {
      item->setIcon(LocalOmniIconUrl(*m_theme.icon).withFallback(BuiltinOmniIconUrl("vicinae")));
    } else {
      item->setIcon(BuiltinOmniIconUrl("vicinae"));
    }

    std::vector<ColorLike> colors{m_theme.colors.red,     m_theme.colors.blue,   m_theme.colors.green,
                                  m_theme.colors.magenta, m_theme.colors.purple, m_theme.colors.orange,
                                  m_theme.colors.text,    m_theme.colors.subtext};

    item->setColors(colors);
    item->setStrokeColor(m_theme.colors.text);

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

  ActionPanelView *actionPanel() const override {
    auto panel = new ActionPanelStaticListView;
    auto setTheme = new SetThemeAction(m_theme.id);

    setTheme->setPrimary(true);
    setTheme->setShortcut({.key = "return"});
    panel->setTitle(m_theme.name);
    panel->addAction(setTheme);

    return panel;
  }

  const ThemeInfo &theme() const { return m_theme; }

  ThemeItem(const ThemeInfo &theme) : m_theme(theme) {}
};

class ManageThemesView : public ListView {
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
    setSearchPlaceholderText("Manage themes...");
  }

  void textChanged(const QString &s) override { generateList(s); }

public:
  ManageThemesView() {
    ThemeService::instance().scanThemeDirectories();
    /*
connect(&ThemeService::instance(), &ThemeService::themeChanged, this,
        [this](const auto &info) { generateList(searchText()); });
    */
  }
};
