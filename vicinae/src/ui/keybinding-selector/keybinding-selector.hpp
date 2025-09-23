#pragma once
#include "../image/url.hpp"
#include "ui/form/selector-input.hpp"

struct KeyBindingInfo {
  QString id;
  QString name;
  QString description;
};

class KeyBindingSelectorItem : public SelectorInput::AbstractItem {
  KeyBindingInfo m_keybinding;

  QString displayName() const override { return m_keybinding.name; }

  QString generateId() const override { return m_keybinding.id; }

  std::optional<ImageURL> icon() const override {
    return ImageURL::builtin("keyboard");
  }

  AbstractItem *clone() const override { return new KeyBindingSelectorItem(*this); }

public:
  const KeyBindingInfo &keybinding() const { return m_keybinding; }
  KeyBindingSelectorItem(const KeyBindingInfo &keybinding) : m_keybinding(keybinding) {}
};

class KeyBindingSelector : public SelectorInput {
public:
  KeyBindingSelector();
};