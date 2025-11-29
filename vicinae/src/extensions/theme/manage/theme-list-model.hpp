#include "layout.hpp"
#include "services/config/config-service.hpp"
#include "theme.hpp"
#include "ui/color-circle/color_circle.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/image/image.hpp"
#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include "ui/vlist/common/section-model.hpp"
#include "theme/theme-file.hpp"
#include <qnamespace.h>

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

enum class ThemeSection { Current, Available };

class ThemeListModel : public vicinae::ui::SectionListModel<std::shared_ptr<ThemeFile>, ThemeSection> {
public:
  ThemeListModel(ConfigService *configService, ThemeService *themeService, QObject *parent = nullptr)
      : m_themeService(themeService), m_configService(configService) {
    setParent(parent);
    regenerateThemes();
    connect(m_configService, &ConfigService::configChanged, this,
            [this](auto &&next, auto &&prev) { regenerateThemes(); });
  }

  void regenerateThemes() { setFilter(m_query); }

  void setFilter(const QString &query) {
    m_query = query;
    auto themes = m_themeService->themes();

    m_themes.clear();
    m_selectedTheme.reset();
    m_themes.reserve(themes.size() - 1);

    for (auto &theme : themes) {
      if (!theme->name().contains(query, Qt::CaseInsensitive)) continue;
      if (theme->id() == m_configService->value().theme.name) {
        m_selectedTheme = std::move(theme);
      } else {
        m_themes.emplace_back(std::move(theme));
      }
    }

    emit dataChanged();
  }

  int sectionCount() const override { return 2; }

  ThemeSection sectionIdFromIndex(int idx) const override {
    if (idx == 0) return ThemeSection::Current;
    return ThemeSection::Available;
  }

  int sectionItemHeight(ThemeSection id) const override { return 60; }

  std::string_view sectionName(ThemeSection id) const override {
    switch (id) {
    case ThemeSection::Current:
      return "Current Theme";
    case ThemeSection::Available:
      return "Available Themes";
    }
    return "";
  }

  Item sectionItemAt(ThemeSection id, int itemIdx) const override {
    switch (id) {
    case ThemeSection::Current:
      return m_selectedTheme.value();
    default:
      return m_themes[itemIdx];
    }
  }

  int sectionItemCount(ThemeSection id) const override {
    switch (id) {
    case ThemeSection::Current:
      return m_selectedTheme.has_value() * 1;
    case ThemeSection::Available:
      return m_themes.size();
    }
  }

  WidgetType *createItemWidget(const Item &type) const override { return new ThemeItemWidget; }

  void refreshItemWidget(const Item &theme, WidgetType *widget) const override {
    auto item = static_cast<ThemeItemWidget *>(widget);

    item->setTitle(theme->name());
    item->setDescription(theme->description().isEmpty() ? "Default theme description" : theme->description());

    if (theme->icon()) {
      item->setIcon(ImageURL::local(*theme->icon()).withFallback(ImageURL::builtin("vicinae")));
    } else {
      item->setIcon(ImageURL::builtin("vicinae"));
    }

    std::vector<ColorLike> colors{
        theme->resolve(SemanticColor::Red),        theme->resolve(SemanticColor::Blue),
        theme->resolve(SemanticColor::Cyan),       theme->resolve(SemanticColor::Green),
        theme->resolve(SemanticColor::Magenta),    theme->resolve(SemanticColor::Orange),
        theme->resolve(SemanticColor::Foreground), theme->resolve(SemanticColor::TextMuted)};

    item->setColors(colors);
  }

  WidgetTag widgetTag(const Item &item) const override { return 1; }

  StableID stableId(const Item &item) const override {
    static std::hash<QString> hasher = {};
    return hasher(item->id());
  }

private:
  QString m_query;
  ThemeService *m_themeService = nullptr;
  ConfigService *m_configService = nullptr;
  std::optional<Item> m_selectedTheme;
  std::vector<Item> m_themes;
};
