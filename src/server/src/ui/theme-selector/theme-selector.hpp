#pragma once
#include "ui/image/url.hpp"
#include "theme/theme-file.hpp"
#include "ui/form/selector-input.hpp"

class ThemeSelectorItem : public SelectorInput::AbstractItem {

public:
  const ThemeFile &theme() const { return *m_theme; }
  ThemeSelectorItem(const std::shared_ptr<ThemeFile> &theme) : m_theme(theme) {}

private:
  QString displayName() const override { return m_theme->name(); }

  QString generateId() const override { return m_theme->id(); }

  std::optional<ImageURL> icon() const override {
    if (m_theme->icon()) return ImageURL::local(*m_theme->icon());
    return ImageURL::builtin("vicinae");
  }

  AbstractItem *clone() const override { return new ThemeSelectorItem(*this); }

  std::shared_ptr<ThemeFile> m_theme;
};

class ThemeSelector : public SelectorInput {
public:
  ThemeSelector();
};
