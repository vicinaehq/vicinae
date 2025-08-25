#pragma once
#include "extend/form-model.hpp"
#include "extension/form/extension-form-input.hpp"
#include "ui/form/base-input.hpp"

class ExtensionTextField : public ExtensionFormInput {
  BaseInput *m_input = new BaseInput(this);
  std::shared_ptr<FormModel::TextField> m_model;

public:
  void handleTextChanged(const QString &text) {
    if (m_model->onChange) { m_extensionNotifier->notify(*m_model->onChange, text); }
  }

  void clear() override { m_input->setText(""); }

  QJsonValue asJsonValue() const override { return m_input->text(); }

  void setValueAsJson(const QJsonValue &value) override { m_input->setText(value.toString()); }

  void render(const std::shared_ptr<FormModel::IField> &field) override {
    m_model = std::static_pointer_cast<FormModel::TextField>(field);

    if (auto placeholder = m_model->m_placeholder) m_input->setPlaceholderText(*placeholder);
    if (auto value = m_model->value) { m_input->setText(value->toString()); }
  }

  ExtensionTextField() {
    setWrapped(m_input, m_input->focusNotifier());
    connect(m_input, &BaseInput::textChanged, this, &ExtensionTextField::handleTextChanged);
  }
};
