#pragma once
#include "common.hpp"
#include "preference.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/selector-input.hpp"
#include <qjsonvalue.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qwidget.h>

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

struct PreferenceVisitor {
  Preference m_preference;

  PreferenceVisitor(const Preference &preference) : m_preference(preference) {}
};

class PreferenceField : public FormField, public JsonFormItemWidget {
  JsonFormItemWidget *m_serializable = nullptr;

  Preference m_preference;

  void setWidget(QWidget *widget, JsonFormItemWidget *serializable) {
    m_serializable = serializable;
    FormField::setWidget(widget);
  }

public:
  const auto &preference() const { return m_preference; }
  QJsonValue asJsonValue() const override {
    return m_serializable ? m_serializable->asJsonValue() : QJsonValue();
  }

  void setValueAsJson(const QJsonValue &value) override {
    if (m_serializable) m_serializable->setValueAsJson(value);
  }

  PreferenceField(const Preference &preference) : m_preference(preference) {
    /*
setName(preference->title());

if (preference->isTextType()) {
auto textPreference = std::static_pointer_cast<TextFieldPreference>(preference);
auto input = new BaseInput;

input->setPlaceholderText(textPreference->placeholder());

if (auto dflt = textPreference->defaultValueAsJson(); !dflt.isNull()) {
  input->setText(dflt.toString());
}

if (preference->isPasswordType()) { input->setEchoMode(QLineEdit::Password); }

setWidget(input, input);
return;
}

if (preference->isDropdownType()) {
auto dropdownPreference = std::static_pointer_cast<DropdownPreference>(preference);
auto input = new SelectorInput;

input->list()->updateModel([&]() {
  auto map = [](auto &&option) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
    return std::make_unique<PreferenceDropdownItem>(option);
  };
  auto items =
      dropdownPreference->options() | std::views::transform(map) | std::ranges::to<std::vector>();
  auto &section = input->list()->addSection();

  section.addItems(std::move(items));
});

if (auto dflt = dropdownPreference->defaultValueAsJson(); !dflt.isNull()) {
  input->setValue(dflt.toString());
}

setWidget(input, input);
return;
}

if (preference->isCheckboxType()) {
auto preference = new CheckboxInput;

setWidget(preference, preference);
}
  */

    qCritical() << "preference" << preference.name() << "has unknown type";
  }
};
