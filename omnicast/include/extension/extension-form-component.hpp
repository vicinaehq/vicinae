#pragma once
#include "app.hpp"
#include "common.hpp"
#include "extend/form-model.hpp"
#include "extend/model.hpp"
#include "extension/extension-component.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ExtensionFormField : public FormField {
  Q_OBJECT

  IJsonFormField *m_jsonField = nullptr;
  FormModel::Field m_model = FormModel::InvalidField();
  const FormModel::FieldBase *m_model_base;

public:
  const FormModel::Field model() const { return m_model; }

  QString id() const { return m_model_base->id; }
  bool autoFocusable() const { return m_model_base->autoFocus; }
  QJsonValue valueAsJson() const { return m_jsonField ? m_jsonField->asJsonValue() : QJsonValue(); }

  void setModel(const FormModel::Field &model) {
    bool isSameType = model.index() == m_model.index();

    m_model = model;
    m_model_base = std::visit([](const FormModel::FieldBase &base) { return &base; }, m_model);
    setName(m_model_base->id);

    if (m_model_base->error) { setError(*m_model_base->error); }

    if (auto textField = std::get_if<FormModel::TextFieldModel>(&model)) {
      if (!isSameType) {
        auto input = new BaseInput;

        m_jsonField = input;

        connect(input, &BaseInput::textChanged, this, [this](const QString &text) {
          if (auto onChange = m_model_base->onChange) { emit notifyEvent(*onChange, {text}); }
        });

        if (auto w = widget()) { w->deleteLater(); }
        setWidget(input);
      }

      auto input = static_cast<BaseInput *>(widget());

      if (auto placeholder = textField->placeholder) { input->setPlaceholderText(*placeholder); }
      if (auto value = textField->value) { input->setText(value->toString()); }
    }

    if (auto checkboxField = std::get_if<FormModel::CheckboxFieldModel>(&model)) {
      if (!isSameType) {
        auto input = new CheckboxInput;

        m_jsonField = input;
        connect(input, &CheckboxInput::valueChanged, this, [this](bool value) {
          qDebug() << "onChange" << m_model_base->id;
          qDebug() << "onChange value" << m_model_base->onChange;

          if (auto onChange = m_model_base->onChange) { emit notifyEvent(*onChange, {value}); }
        });

        if (auto w = widget()) { w->deleteLater(); }
        setWidget(input);
      }

      auto checkbox = static_cast<CheckboxInput *>(widget());

      if (auto value = checkboxField->value) { checkbox->setValueAsJson(value->toBool()); }
    }
  }

  void handleFocusChanged(bool value) {
    if (!value && m_model_base->onBlur) { emit notifyEvent(*m_model_base->onBlur, {}); }
    if (value && m_model_base->onFocus) { emit notifyEvent(*m_model_base->onFocus, {}); }
  }

  ExtensionFormField() : m_model(FormModel::InvalidField{}) {
    connect(this, &ExtensionFormField::focusChanged, this, &ExtensionFormField::handleFocusChanged);
  }

signals:
  void notifyEvent(const QString &handler, const std::vector<QJsonValue> &args) const;
};

class ExtensionFormComponent : public AbstractExtensionRootComponent {
  FormWidget *m_form = new FormWidget;
  std::unordered_map<QString, ExtensionFormField *> m_fieldMap;
  std::vector<ExtensionFormField *> m_fields;
  QVBoxLayout *m_layout = new QVBoxLayout;
  bool autoFocused = false;

public:
  void handleSubmit(const EventHandler &handler) const {
    QJsonObject payload;

    for (const auto &field : m_fields) {
      if (field->hasError()) return;

      qDebug() << "Submit" << field->valueAsJson();

      payload[field->id()] = field->valueAsJson();
    }

    notifyEvent(handler, {payload});
  }

  void clear() {
    while (m_layout->count() > 0) {
      if (auto item = m_layout->takeAt(0)) {}
    }
  }

  void render(const RenderModel &model) override {
    auto formModel = std::get<FormModel>(model);
    size_t i = 0;

    if (auto pannel = formModel.actions) {
      for (auto &item : pannel->children) {
        if (auto action = std::get_if<ActionModel>(&item)) {
          if (i == 0) action->shortcut = {"return", {"shift"}};

          ++i;
        }
      }

      emit updateActionPannel(*pannel);
    }

    std::vector<QString> visibleIds;

    m_fields.clear();

    QWidget *lastAutoFocusable = nullptr;
    bool hasFocus = false;

    for (int i = 0; i != formModel.items.size(); ++i) {
      if (auto field = std::get_if<FormModel::Field>(&formModel.items.at(i))) {
        std::visit(
            [this, field, i, &lastAutoFocusable, &visibleIds](const FormModel::FieldBase &base) {
              ExtensionFormField *formField = nullptr;

              visibleIds.push_back(base.id);

              if (auto it = m_fieldMap.find(base.id); it != m_fieldMap.end()) {
                formField = it->second;
              } else {
                formField = new ExtensionFormField();
                m_fieldMap.insert({base.id, formField});
                connect(formField, &ExtensionFormField::notifyEvent, this,
                        &ExtensionFormComponent::notifyEvent);
              }

              formField->setModel(*field);
              m_fields.emplace_back(formField);

              if (m_layout->count() > i) {
                m_layout->replaceWidget(m_layout->itemAt(i)->widget(), formField);
              } else {
                m_layout->addWidget(formField, 0, Qt::AlignTop);
              }
            },
            *field);
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
        qDebug() << "erase" << it->first;
        it->second->deleteLater();
        it = m_fieldMap.erase(it);
      } else {
        ++it;
      }
    }

    qDebug() << "form model render with" << formModel.items.size() << "items";
  }

  ExtensionFormComponent(AppWindow &app) : AbstractExtensionRootComponent(app) {
    m_layout->setAlignment(Qt::AlignTop);
    setLayout(m_layout);
  }
};
