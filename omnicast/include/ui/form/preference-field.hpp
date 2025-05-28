#pragma once
#include "common.hpp"
#include "preference.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/selector-input.hpp"
#include <memory>
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
  PreferenceDropdownItem(const DropdownPreference::Option &option)
      : m_id(option.value), m_displayName(option.title) {}
};

class PreferenceField : public FormField, public IJsonFormField {
  IJsonFormField *m_serializable = nullptr;

  std::shared_ptr<BasePreference> m_preference;

  void setWidget(QWidget *widget, IJsonFormField *serializable) {
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

  PreferenceField(const std::shared_ptr<BasePreference> &preference) : m_preference(preference) {
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

      input->beginUpdate();

      for (const auto &option : dropdownPreference->options()) {
        input->addItem(std::make_unique<PreferenceDropdownItem>(option));
      }

      input->commitUpdate();

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

    qCritical() << "preference" << preference->name() << "has unknown type";
  }
};
