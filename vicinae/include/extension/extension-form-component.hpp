#pragma once
#include "extend/form-model.hpp"
#include "extension/extension-view.hpp"
#include "extension/form/extension-checkbox-field.hpp"
#include "extension/form/extension-text-field.hpp"
#include "ui/form/form-field.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include <qboxlayout.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qnamespace.h>
#include <qscrollarea.h>
#include <qtmetamacros.h>
#include "extension/form/extension-form-input.hpp"
#include "extension/form/extension-form-input.hpp"
#include "extension/form/extension-dropdown.hpp"
#include "extension/form/extension-password-field.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include <qwidget.h>

class ExtensionFormField : public FormField {
  Q_OBJECT

  std::shared_ptr<FormModel::IField> m_model;
  ExtensionFormInput *m_widget = nullptr;

  static ExtensionFormInput *createFieldWidget(const FormModel::IField *field) {
    // XXX awful, we will fix this very soon
    if (auto f = dynamic_cast<const FormModel::CheckboxField *>(field)) { return new ExtensionCheckboxField; }
    if (auto f = dynamic_cast<const FormModel::TextField *>(field)) { return new ExtensionTextField; }
    if (auto f = dynamic_cast<const FormModel::DropdownField *>(field)) { return new ExtensionDropdown; }
    if (auto f = dynamic_cast<const FormModel::PasswordField *>(field)) { return new ExtensionPasswordField; }

    return nullptr;
  }

public:
  const FormModel::IField *model() const { return m_model.get(); }

  QString id() const { return m_model->id; }
  bool autoFocusable() const { return m_model->autoFocus; }
  QJsonValue valueAsJson() const { return m_widget ? m_widget->asJsonValue() : QJsonValue(); }

  void reset() {
    if (m_widget) { m_widget->reset(); }
  };

  void setModel(const std::shared_ptr<FormModel::IField> &model) {
    bool isSameType = m_model && m_model->fieldTypeId() == model->fieldTypeId();

    m_model = model;

    if (!isSameType) {
      m_widget = createFieldWidget(model.get());

      connect(m_widget->m_extensionNotifier, &ExtensionEventNotifier::eventNotified, this,
              &ExtensionFormField::notifyEvent);

      if (auto old = widget()) old->deleteLater();

      setWidget(m_widget, m_widget->focusNotifier());
    }

    if (model->title) { setName(*model->title); }
    if (model->error) { setError(*model->error); }

    m_widget->dispatchRender(model);

    // initialize default value the first time
    if (auto value = m_model->defaultValue; value && !isSameType) { m_widget->setValueAsJson(*value); }
  }

  void handleFocusChanged(bool value) {
    if (!value && m_model->onBlur) { emit notifyEvent(*m_model->onBlur, {}); }
    if (value && m_model->onFocus) { emit notifyEvent(*m_model->onFocus, {}); }
  }

  ExtensionFormField() {
    connect(this, &ExtensionFormField::focusChanged, this, &ExtensionFormField::handleFocusChanged);
  }

signals:
  void notifyEvent(const QString &handler, const QJsonArray &args) const;
};

class ExtensionFormComponent : public ExtensionSimpleView {
  QScrollArea *m_scrollArea = new VerticalScrollArea(this);
  std::unordered_map<QString, ExtensionFormField *> m_fieldMap;
  std::vector<ExtensionFormField *> m_fields;
  QVBoxLayout *m_layout = new QVBoxLayout;
  bool autoFocused = false;

  bool supportsSearch() const override { return false; }

public:
  std::expected<QJsonObject, QString> submit() override {
    QJsonObject payload;

    for (const auto &field : m_fields) {
      if (field->hasError()) return std::unexpected("one or more fields have error");

      payload[field->id()] = field->valueAsJson();
    }

    reset();
    return payload;
  }

  void reset() {
    for (const auto &field : m_fields) {
      field->reset();
    }
  }

  void render(const RenderModel &model) override {
    auto formModel = std::get<FormModel>(model);
    size_t i = 0;

    if (auto pannel = formModel.actions) { setActionPanel(*pannel); }

    std::vector<QString> visibleIds;

    m_fields.clear();

    QWidget *lastAutoFocusable = nullptr;
    bool hasFocus = false;

    for (int i = 0; i != formModel.items.size(); ++i) {
      if (auto f = std::get_if<std::shared_ptr<FormModel::IField>>(&formModel.items.at(i))) {
        auto &field = *f;
        ExtensionFormField *formField = nullptr;

        visibleIds.push_back(field->id);

        if (auto it = m_fieldMap.find(field->id); it != m_fieldMap.end()) {
          formField = it->second;
        } else {
          formField = new ExtensionFormField();
          m_fieldMap.insert({field->id, formField});
          connect(formField, &ExtensionFormField::notifyEvent, this, &ExtensionFormComponent::notify);
        }

        formField->setModel(field);
        m_fields.emplace_back(formField);

        if (m_layout->count() > i) {
          m_layout->replaceWidget(m_layout->itemAt(i)->widget(), formField);
        } else {
          m_layout->addWidget(formField, 0, Qt::AlignTop);
        }
      }
    }

    if (m_fields.empty()) { autoFocused = false; }

    for (int i = 0; i < m_fields.size(); ++i) {
      auto field = m_fields[i];

      if (!autoFocused && field->autoFocusable()) {
        field->focus();
        autoFocused = true;
      }
      if (i < m_fields.size() - 1) setTabOrder(m_fields[i], m_fields[i + 1]);
    }

    if (!m_fields.empty() && !autoFocused) {
      m_fields[0]->focus();
      autoFocused = true;
    }

    for (auto it = m_fieldMap.begin(); it != m_fieldMap.end();) {
      if (std::find(visibleIds.begin(), visibleIds.end(), it->first) == visibleIds.end()) {
        it->second->deleteLater();
        it = m_fieldMap.erase(it);
      } else {
        ++it;
      }
    }
  }

  ExtensionFormComponent() {
    auto layout = new QVBoxLayout;
    auto form = new QWidget;

    setDefaultActionShortcuts({KeyboardShortcutModel::submit()});

    m_layout->setAlignment(Qt::AlignTop);
    form->setLayout(m_layout);

    m_scrollArea->setVerticalScrollBar(new OmniScrollBar);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(form);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setAutoFillBackground(false);
    form->setAutoFillBackground(false);
    m_scrollArea->setAttribute(Qt::WA_TranslucentBackground);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_scrollArea);
    setLayout(layout);
  }
};
