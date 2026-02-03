#pragma once
#include "preference.hpp"
#include "ui/file-picker/file-picker.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/preference-dropdown/preference-dropdown.hpp"
#include <qjsonvalue.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qwidget.h>

struct PreferenceFieldWidgetVisitor {
  Preference m_preference;

  BaseInput *createInput() {
    auto input = new BaseInput;
    input->setPlaceholderText(m_preference.placeholder());
    return input;
  }

  JsonFormItemWidget *operator()(const auto &other) { return createInput(); }
  JsonFormItemWidget *operator()(const Preference::FilePickerData &text) { return new FilePicker; }
  JsonFormItemWidget *operator()(const Preference::DirectoryPickerData &text) {
    auto picker = new FilePicker;
    picker->setOnlyDirectories();
    return picker;
  }
  JsonFormItemWidget *operator()(const Preference::TextData &text) { return createInput(); }
  JsonFormItemWidget *operator()(const Preference::PasswordData &password) {
    auto input = createInput();
    input->setEchoMode(QLineEdit::Password);
    return input;
  }
  JsonFormItemWidget *operator()(const Preference::DropdownData &data) {
    auto selector = new PreferenceDropdown;

    selector->setOptions(data.options);

    return selector;
  }
  JsonFormItemWidget *operator()(const Preference::CheckboxData &data) {
    auto input = new CheckboxInput;

    return input;
  }
  JsonFormItemWidget *operator()(const Preference::UnknownData &data) { return nullptr; }

public:
  PreferenceFieldWidgetVisitor(Preference preference) : m_preference(preference) {}
};

class PreferenceField : public FormField {
  JsonFormItemWidget *m_serializable = nullptr;
  Preference m_preference;

  void setWidget(JsonFormItemWidget *widget) {
    m_serializable = widget;
    FormField::setWidget(widget, widget->focusNotifier());
  }

public:
  const auto &preference() const { return m_preference; }

  JsonFormItemWidget *widget() { return m_serializable; }

  PreferenceField(const Preference &preference) : m_preference(preference) {
    setName(preference.title());
    auto widget = std::visit(PreferenceFieldWidgetVisitor(preference), preference.data());

    if (!widget) return;

    setWidget(widget);
  }
};
