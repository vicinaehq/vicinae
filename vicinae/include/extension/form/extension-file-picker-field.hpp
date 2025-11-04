#pragma once
#include "extend/form-model.hpp"
#include "extension/form/extension-form-input.hpp"
#include "ui/file-picker/file-picker.hpp"

class ExtensionFilePickerField : public ExtensionFormInput {
  FilePicker *m_picker = new FilePicker(this);
  std::shared_ptr<FormModel::FilePickerField> m_model;

  void handleChange() {
    if (m_model && m_model->onChange) {
      m_extensionNotifier->notify(*m_model->onChange, m_picker->asJsonValue());
    }
  }

  void setValueFromJson(const QJsonValue &value) {
    if (value.isNull() || value.isUndefined()) {
      m_picker->clear();
      return;
    }

    if (value.isArray()) {
      m_picker->clear();
      for (const auto &file : value.toArray()) {
        m_picker->addFile(file.toString().toStdString());
      }
    } else if (value.isString()) {
      m_picker->setFile(value.toString().toStdString());
    }
  }

public:
  void clear() override { m_picker->clear(); }

  QJsonValue asJsonValue() const override { return m_picker->asJsonValue(); }

  void setValueAsJson(const QJsonValue &value) override { setValueFromJson(value); }

  void render(const std::shared_ptr<FormModel::IField> &field) override {
    m_model = std::static_pointer_cast<FormModel::FilePickerField>(field);

    m_picker->setMultiple(m_model->allowMultipleSelection);

    if (m_model->canChooseDirectories && !m_model->canChooseFiles) {
      m_picker->setOnlyDirectories();
    } else {
      m_picker->setMimeTypeFilters({});
    }

    if (auto value = m_model->value) { setValueFromJson(*value); }
  }

  ExtensionFilePickerField(QWidget *parent = nullptr) : ExtensionFormInput(parent) {
    setWrapped(m_picker, m_picker->focusNotifier());
    connect(m_picker, &FilePicker::valueChanged, this, &ExtensionFilePickerField::handleChange);
  }
};
