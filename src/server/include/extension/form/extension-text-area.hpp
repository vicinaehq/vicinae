#pragma once
#include "extend/form-model.hpp"
#include "extension/form/extension-form-input.hpp"
#include "ui/form/text-area.hpp"

class ExtensionTextArea : public ExtensionFormInput {

  TextArea *m_area = new TextArea(this);
  std::shared_ptr<FormModel::TextAreaField> m_model;

  void handleTextChanged() {
    if (m_model && m_model->onChange) { m_extensionNotifier->notify(*m_model->onChange, m_area->text()); }
  }

public:
  void clear() override { m_area->setText(""); }

  QJsonValue asJsonValue() const override { return m_area->text(); }

  void setValueAsJson(const QJsonValue &value) override { m_area->setText(value.toString()); }

  void render(const std::shared_ptr<FormModel::IField> &field) override {
    m_model = std::static_pointer_cast<FormModel::TextAreaField>(field);

    if (auto placeholder = m_model->placeholder) m_area->setPlaceholderText(*placeholder);
    if (auto value = m_model->value) { m_area->setText(value->toString()); }
  }

  ExtensionTextArea() {
    setWrapped(m_area, m_area->focusNotifier());
    connect(m_area->textEdit(), &QPlainTextEdit::textChanged, this, &ExtensionTextArea::handleTextChanged);
  }
};
