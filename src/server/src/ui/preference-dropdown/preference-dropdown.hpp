#pragma once
#include "preference.hpp"
#include "ui/form/selector-input.hpp"

class PreferenceDropdown : public SelectorInput {
public:
  void setOptions(const std::vector<Preference::DropdownData::Option> &opts);

  PreferenceDropdown(QWidget *parent = nullptr);
};

class PreferenceDropdownItem : public SelectorInput::AbstractItem {
  QString m_id;
  QString m_displayName;

  QString displayName() const override { return m_displayName; }

  QString generateId() const override { return m_id; }

  AbstractItem *clone() const override { return new PreferenceDropdownItem(*this); }

public:
  PreferenceDropdownItem(const Preference::DropdownData::Option &option)
      : m_id(option.value), m_displayName(option.title) {}
};
