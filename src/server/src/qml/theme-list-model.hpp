#pragma once
#include "section-source.hpp"
#include "theme/theme-file.hpp"
#include <functional>
#include <memory>
#include <vector>

class ThemeSection : public SectionSource {
public:
  enum ExtraRole {
    PaletteColor0 = 100,
    PaletteColor1,
    PaletteColor2,
    PaletteColor3,
    PaletteColor4,
    PaletteColor5,
    PaletteColor6,
    PaletteColor7,
  };

  void setThemes(const QString &name, std::vector<std::shared_ptr<ThemeFile>> themes);

  void setOnThemeSelected(std::function<void(const std::shared_ptr<ThemeFile> &)> cb) {
    m_onThemeSelected = std::move(cb);
  }

  QString sectionName() const override { return m_name; }
  int count() const override { return static_cast<int>(m_themes.size()); }

  void onSelected(int i) override {
    if (m_onThemeSelected) m_onThemeSelected(m_themes[i]);
  }

  QVariant customData(int i, int role) const override;
  QHash<int, QByteArray> customRoleNames() const override;

protected:
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  QString m_name;
  std::vector<std::shared_ptr<ThemeFile>> m_themes;
  std::function<void(const std::shared_ptr<ThemeFile> &)> m_onThemeSelected;
};
