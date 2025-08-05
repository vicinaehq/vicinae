#pragma once
#include "../image/url.hpp"
#include "ui/form/selector-input.hpp"

class QThemeSelectorItem : public SelectorInput::AbstractItem {
  QString m_theme;

  QString displayName() const override { return m_theme; }

  QString generateId() const override { return m_theme; }

  std::optional<ImageURL> icon() const override { return ImageURL::system("folder").param("theme", m_theme); }

  AbstractItem *clone() const override { return new QThemeSelectorItem(*this); }

public:
  const QString &theme() const { return m_theme; }

  QThemeSelectorItem(const QString &path) : m_theme(path) {}
};

class QThemeSelector : public SelectorInput {
public:
  QThemeSelector();
};
