#pragma once
#include <qboxlayout.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qnamespace.h>
#include <qscrollarea.h>
#include <qlabel.h>
#include <qtmetamacros.h>
#include <qurl.h>
#include <qwidget.h>
#include "extension/form/extension-form-input.hpp"
#include "extension/form/extension-form-input.hpp"
#include "extension/form/extension-dropdown.hpp"
#include "extension/form/extension-password-field.hpp"
#include "extension/form/extension-date-picker.hpp"
#include "extension/form/extension-file-picker-field.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include "extend/form-model.hpp"
#include "extension/extension-view.hpp"
#include "extension/form/extension-checkbox-field.hpp"
#include "extension/form/extension-text-field.hpp"
#include "extension/form/extension-text-area.hpp"
#include "ui/form/form-field.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include "ui/typography/typography.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "common.hpp"

class ExtensionFormField : public FormField {
  Q_OBJECT

  std::shared_ptr<FormModel::IField> m_model;
  ExtensionFormInput *m_widget = nullptr;

  static ExtensionFormInput *createFieldWidget(const FormModel::IField *field) {
    // XXX awful, we will fix this very soon
    if (auto f = dynamic_cast<const FormModel::CheckboxField *>(field)) { return new ExtensionCheckboxField; }
    if (auto f = dynamic_cast<const FormModel::TextField *>(field)) { return new ExtensionTextField; }
    if (auto f = dynamic_cast<const FormModel::TextAreaField *>(field)) { return new ExtensionTextArea; }
    if (auto f = dynamic_cast<const FormModel::DropdownField *>(field)) { return new ExtensionDropdown; }
    if (auto f = dynamic_cast<const FormModel::PasswordField *>(field)) { return new ExtensionPasswordField; }
    if (auto f = dynamic_cast<const FormModel::DatePickerField *>(field)) { return new ExtensionDatePicker; }
    if (auto f = dynamic_cast<const FormModel::FilePickerField *>(field)) {
      return new ExtensionFilePickerField;
    }

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
              &ExtensionFormField::notifyEvent, Qt::QueuedConnection);

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
    if (!value && m_model->onBlur) {
      QMetaObject::invokeMethod(
          this, [this]() { emit notifyEvent(*m_model->onBlur, {}); }, Qt::QueuedConnection);
    }
    if (value && m_model->onFocus) {
      QMetaObject::invokeMethod(
          this, [this]() { emit notifyEvent(*m_model->onFocus, {}); }, Qt::QueuedConnection);
    }
  }

  ExtensionFormField() {
    connect(this, &ExtensionFormField::focusChanged, this, &ExtensionFormField::handleFocusChanged,
            Qt::QueuedConnection);
  }

signals:
  void notifyEvent(const QString &handler, const QJsonArray &args) const;
};

class FormDescriptionWidget : public JsonFormItemWidget {
  QVBoxLayout *m_layout = new QVBoxLayout(this);

public:
  FormDescriptionWidget(const QString &text) {
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);

    auto body = new TypographyWidget;
    body->setText(text);
    body->setColor(SemanticColor::TextMuted);
    body->setWordWrap(true);
    m_layout->addWidget(body);
  }

  QJsonValue asJsonValue() const override { return QJsonValue(); }
  void setValueAsJson(const QJsonValue &) override {}
  FocusNotifier *focusNotifier() const override { return nullptr; }
};

class FormLinkAccessoryWidget : public QWidget {
  QWidget *m_inner = new QWidget(this);
  QLabel *m_text = new QLabel(m_inner);
  QUrl m_href;

  void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::LeftButton && !m_href.isEmpty()) {
      ServiceRegistry::instance()->appDb()->openTarget(m_href.toString());
    }
    QWidget::mousePressEvent(event);
  }

public:
  FormLinkAccessoryWidget() {
    // Inner layout that holds the text and sizes to content
    auto innerLayout = new QHBoxLayout(m_inner);
    innerLayout->setContentsMargins(10, 0, 10, 0);
    innerLayout->addWidget(m_text);
    m_inner->setLayout(innerLayout);
    m_inner->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Outer layout that aligns inner widget to right
    auto outerLayout = new QHBoxLayout(this);
    outerLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(m_inner);
  }

  void setText(const QString &text) { m_text->setText(text); }
  void setHref(const QUrl &href) { m_href = href; }
};

class ExtensionFormComponent : public ExtensionSimpleView {
  QScrollArea *m_scrollArea = new VerticalScrollArea(this);
  std::unordered_map<QString, ExtensionFormField *> m_fieldMap;
  std::vector<ExtensionFormField *> m_fields;
  QVBoxLayout *m_layout = new QVBoxLayout;
  bool autoFocused = false;
  FormLinkAccessoryWidget *m_linkAccessory = new FormLinkAccessoryWidget;

  bool supportsSearch() const override { return false; }
  QWidget *searchBarAccessory() const override { return m_linkAccessory; }

private:
  QWidget *createDescriptionField(const FormModel::Description &d) const {
    auto field = new FormField;
    if (d.title) {
      field->setName(*d.title);
    } else {
      field->setName("");
    }
    field->setWidget(new FormDescriptionWidget(d.text));
    return field;
  }

public:
  tl::expected<QJsonObject, QString> submit() override {
    QJsonObject payload;

    for (const auto &field : m_fields) {
      if (field->hasError()) return tl::unexpected("one or more fields have error");

      payload[field->id()] = field->valueAsJson();
    }

    // reset();
    return payload;
  }

  void reset() {
    for (const auto &field : m_fields) {
      field->reset();
    }
  }

  void render(const RenderModel &model) override {
    auto formModel = std::get<FormModel>(model);

    // Link Accessory
    if (auto accessory = formModel.searchBarAccessory) {
      if (auto link = std::get_if<FormModel::LinkAccessoryModel>(&*accessory)) {
        m_linkAccessory->setText(link->text);
        m_linkAccessory->setHref(QUrl(link->target));
      }
    }
    setSearchAccessoryVisiblity(formModel.searchBarAccessory.has_value() && isVisible());

    size_t i = 0;

    if (auto pannel = formModel.actions) { setActionPanel(*pannel); }

    std::vector<QString> visibleIds;

    m_fields.clear();

    QWidget *lastAutoFocusable = nullptr;
    bool hasFocus = false;

    int addedWidgets = 0;
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
        ++addedWidgets;
      } else if (auto d = std::get_if<FormModel::Description>(&formModel.items.at(i))) {
        auto field = createDescriptionField(*d);
        if (m_layout->count() > i) {
          if (auto w = m_layout->itemAt(i)->widget()) w->deleteLater();
          m_layout->insertWidget(i, field, 0, Qt::AlignTop);
        } else {
          m_layout->addWidget(field, 0, Qt::AlignTop);
        }
        ++addedWidgets;
      } else if (std::holds_alternative<FormModel::Separator>(formModel.items.at(i))) {
        auto sep = new HDivider;
        if (m_layout->count() > i) {
          if (auto w = m_layout->itemAt(i)->widget()) w->deleteLater();
          m_layout->insertWidget(i, sep);
        } else {
          m_layout->addWidget(sep);
        }
        ++addedWidgets;
      }
    }

    // Trim leftover widgets if the new render has fewer items than before
    while (m_layout->count() > addedWidgets) {
      auto item = m_layout->takeAt(addedWidgets);
      if (item) {
        if (auto w = item->widget()) w->deleteLater();
        delete item;
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

    setDefaultActionShortcuts({Keyboard::Shortcut::submit()});
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
