#pragma once
#include "omni-icon.hpp"
#include "ui/form/selector-input.hpp"
#include "font-service.hpp"

class FontSelectorItem : public SelectorInput::AbstractItem {
  QString m_family;

  QString displayName() const override { return m_family; }

  QString generateId() const override { return m_family; }

  std::optional<OmniIconUrl> icon() const override { return BuiltinOmniIconUrl("text"); }

  AbstractItem *clone() const override { return new FontSelectorItem(*this); }

public:
  QString family() const { return m_family; }
  FontSelectorItem(const QString &family) : m_family(family) {}
};

class FontSelector : public SelectorInput {
public:
  FontSelector();
};
