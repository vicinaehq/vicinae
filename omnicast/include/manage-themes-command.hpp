#pragma once
#include "app.hpp"
#include "base-view.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/color_circle.hpp"
#include "ui/default-list-item-widget.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include "ui/selectable-omni-list-widget.hpp"
#include "ui/typography.hpp"
#include <algorithm>
#include <memory>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
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
      auto circle = new ColorCircle(color, {16, 16});

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
  void setIcon(const OmniIconUrl &url) { m_icon->setUrl(url); }
  void setTitle(const QString &title) { m_title->setText(title); }
  void setDescription(const QString &description) { m_description->setText(description); }
  void setColors(const std::vector<ColorLike> &colors) { m_palette->setColors(colors); }
  void setStrokeColor(const QColor &color) { m_palette->setStrokeColor(color); }

  ThemeItemWidget(QWidget *parent = nullptr) : SelectableOmniListWidget(parent) {
    m_description->setColor(ColorTint::TextSecondary);
    m_textLayout->addWidget(m_title);
    m_textLayout->setContentsMargins(0, 0, 0, 0);
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

class SetThemeAction : public AbstractAction {
  QString _themeName;

  void execute(AppWindow &app) override {
    auto configService = ServiceRegistry::instance()->config();

    configService->updateConfig([&](ConfigService::Value &value) { value.theme.name = _themeName; });

    ThemeService::instance().setTheme(_themeName);
    app.statusBar->setToast("Theme successfully updated");
  }

public:
  SetThemeAction(const QString &themeName)
      : AbstractAction("Set theme", BuiltinOmniIconUrl("brush")), _themeName(themeName) {}
};

class ThemeItem : public OmniList::AbstractVirtualItem, public OmniListView::IActionnable {
  ThemeInfo m_theme;
  std::function<void(const QString &name)> _themeSelectedCallback;

public:
  int calculateHeight(int width) const override {
    static std::unique_ptr<ThemeItemWidget> ruler;

    if (!ruler) {
      ruler = std::make_unique<ThemeItemWidget>();
      ruler->setTitle("dummy title");
      ruler->setDescription("dummy description");
    }

    return ruler->sizeHint().height();
  }

  void setSelectedCallback(const std::function<void(const QString &name)> &cb) {
    _themeSelectedCallback = cb;
  }

  QList<AbstractAction *> generateActions() const override {
    auto set = new SetThemeAction(m_theme.id);

    if (_themeSelectedCallback) {
      set->setExecutionCallback([this]() { _themeSelectedCallback(m_theme.name); });
    }

    return {set};
  }

  QString id() const override { return m_theme.id; }

  bool recyclable() const override { return false; }

  OmniListItemWidget *createWidget() const override {
    auto item = new ThemeItemWidget;

    item->setTitle(m_theme.name);
    item->setDescription(m_theme.description.isEmpty() ? "Default theme description" : m_theme.description);

    if (m_theme.icon) {
      item->setIcon(LocalOmniIconUrl(*m_theme.icon).withFallback(BuiltinOmniIconUrl("omnicast")));
    } else {
      item->setIcon(BuiltinOmniIconUrl("omnicast"));
    }

    std::vector<ColorLike> colors{m_theme.colors.red,     m_theme.colors.blue,   m_theme.colors.green,
                                  m_theme.colors.magenta, m_theme.colors.purple, m_theme.colors.orange,
                                  m_theme.colors.text,    m_theme.colors.subtext};

    item->setColors(colors);
    item->setStrokeColor(m_theme.colors.text);

    return item;
  }

  ThemeItem(const ThemeInfo &theme) : m_theme(theme) {}
};

class ManageThemesView : public ListView {
  void generateList(const QString &query) {
    auto &themeService = ThemeService::instance();
    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> items;
    auto &current = themeService.theme();

    m_list->clearSelection();

    if (current.name.contains(query, Qt::CaseInsensitive)) {
      items.push_back(std::make_unique<OmniList::VirtualSection>("Current Theme"));
      items.push_back(std::make_unique<ThemeItem>(current));
    }

    items.push_back(std::make_unique<OmniList::VirtualSection>("Available Themes"));

    for (const auto &theme : themeService.themes()) {
      if (theme.name == current.name || !theme.name.contains(query, Qt::CaseInsensitive)) continue;
      auto candidate = std::make_unique<ThemeItem>(theme);

      candidate->setSelectedCallback([this, query](const QString &name) {
        m_list->clearSelection();
        generateList(query);
      });

      items.push_back(std::move(candidate));
    }

    m_list->updateFromList(items, OmniList::SelectionPolicy::SelectFirst);
  }

  void onSearchChanged(const QString &s) override { generateList(s); }

public:
  ManageThemesView() {
    setSearchPlaceholderText("Manage themes...");
    ThemeService::instance().scanThemeDirectories();
  }
};
