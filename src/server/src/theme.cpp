#include "theme.hpp"
#include "theme/colors.hpp"
#include "theme/theme-db.hpp"
#include "theme/theme-file.hpp"

void ThemeService::setTheme(const ThemeFile &info) {
  m_theme = std::make_unique<ThemeFile>(info);
  emit themeChanged(info);
}

ThemeService &ThemeService::instance() {
  static ThemeService _instance;
  return _instance;
}

ThemeDatabase &ThemeService::db() { return *m_themeDb; }

ThemeDatabase::ThemeList ThemeService::themes() const { return m_themeDb->themes(); }

const ThemeFile *ThemeService::findTheme(const QString &id) { return m_themeDb->theme(id); }

double ThemeService::pointSize(TextSize size) const {
  switch (size) {
  case TextSize::TextRegular:
    return m_baseFontPointSize;
  case TextSize::TextTitle:
    return m_baseFontPointSize * 1.5;
  case TextSize::TextSmaller:
    return m_baseFontPointSize * 0.9;
  }

  return m_baseFontPointSize;
}

void ThemeService::setFontBasePointSize(double pointSize) { m_baseFontPointSize = pointSize; }

void ThemeService::reloadCurrentTheme() { setTheme(m_theme->id()); }

bool ThemeService::setTheme(const QString &name) {
  if (auto theme = m_themeDb->theme(name)) {
    setTheme(*theme);
    return true;
  }
  return false;
}

const ThemeFile &ThemeService::theme() const { return *m_theme; }

ColorLike ThemeService::getTintColor(SemanticColor tint) const { return m_theme->resolve(tint); }

ThemeService::ThemeService() : m_themeDb(std::make_shared<ThemeDatabase>()) {
  m_themeDb->scan();
  setTheme("vicinae-dark");
  connect(m_themeDb.get(), &ThemeDatabase::themeChanged, this, [this](const ThemeFile &theme) {
    if (m_theme->id() == theme.id()) { setTheme(theme); }
  });
}
