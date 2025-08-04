#pragma once
#include "../image/url.hpp"
#include "theme.hpp"
#include "ui/form/selector-input.hpp"

class ThemeSelectorItem : public SelectorInput::AbstractItem {
  ThemeInfo m_theme;

  QString displayName() const override { return m_theme.name; }

  QString generateId() const override { return m_theme.id; }

  std::optional<ImageURL> icon() const override {
    if (m_theme.icon) return ImageURL::local(*m_theme.icon);
    return ImageURL::builtin("vicinae");
  }

  AbstractItem *clone() const override { return new ThemeSelectorItem(*this); }

public:
  const ThemeInfo &theme() const { return m_theme; }
  ThemeSelectorItem(const ThemeInfo &theme) : m_theme(theme) {}
};

class ThemeSelector : public SelectorInput {
public:
  ThemeSelector();
};
