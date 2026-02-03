#pragma once
#include "extend/form-model.hpp"
#include "extension/form/extension-form-input.hpp"
#include "ui/form/checkbox-input.hpp"

class ExtensionCheckboxField : public ExtensionFormInput {
  CheckboxInput *m_input = new CheckboxInput;
  std::shared_ptr<FormModel::CheckboxField> m_model;

  void clear() override { m_input->setValueAsJson(false); }

  void handleChange(bool value) {
    if (m_model && m_model->onChange) { m_extensionNotifier->notify(*m_model->onChange, value); }
  }

  void setValueAsJson(const QJsonValue &value) override { m_input->setValueAsJson(value); }
  QJsonValue asJsonValue() const override { return m_input->value(); }

public:
  void render(const std::shared_ptr<FormModel::IField> &field) override {
    m_model = std::static_pointer_cast<FormModel::CheckboxField>(field);

    if (m_model->m_label) { m_input->setLabel(*m_model->m_label); }

    if (auto value = m_model->value) { m_input->stealthySetValueAsJson(*value); }
  }

  ExtensionCheckboxField(QWidget *parent = nullptr) : ExtensionFormInput(parent) {
    setWrapped(m_input, m_input->focusNotifier());
    connect(m_input, &CheckboxInput::valueChanged, this, &ExtensionCheckboxField::handleChange);
  }
};
