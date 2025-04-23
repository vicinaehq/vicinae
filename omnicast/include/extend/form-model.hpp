#pragma once
#include "extend/action-model.hpp"
#include "extend/list-model.hpp"
#include "extend/model.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox-input.hpp"
#include "ui/form/selector-input.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qstring.h>
#include <optional>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "ui/focus-notifier.hpp"

class ExtensionEventNotifier : public QObject {
  Q_OBJECT

public:
  ExtensionEventNotifier(QObject *parent = nullptr) : QObject(parent) {}

  void notify(const QString &id, const QJsonValue &value) { emit eventNotified(id, {value}); }

signals:
  void eventNotified(const QString &id, const std::vector<QJsonValue> &value) const;
};

struct FormModel {
  struct FieldBase {
    QString id;
    bool autoFocus;
    std::optional<QJsonValue> defaultValue;
    std::optional<QString> error;
    std::optional<QString> info;
    std::optional<EventHandler> onBlur;
    std::optional<EventHandler> onChange;
    std::optional<EventHandler> onFocus;
    std::optional<QString> placeholder;
    std::optional<QString> title;
    std::optional<QJsonValue> value;
    bool storeValue;
  };

  class IField;

  class JsonFormField : public QWidget {
    QVBoxLayout *m_layout = new QVBoxLayout(this);
    QWidget *m_widget = nullptr;
    FocusNotifier *m_focusNotifier = nullptr;
    std::shared_ptr<IField> m_field;

  protected:
  public:
    ExtensionEventNotifier *m_extensionNotifier = new ExtensionEventNotifier(this);

    virtual QJsonValue jsonValue() const = 0;

    virtual void reset() {
      if (m_field->storeValue) return;
      if (auto value = m_field->defaultValue) { return setJsonValue(*value); }

      clear();
    }

    virtual void clear() {}

    virtual void setJsonValue(const QJsonValue &value) const = 0;

    JsonFormField(QWidget *parent = nullptr) : QWidget(parent) {
      setFocusPolicy(Qt::StrongFocus);
      m_layout->setContentsMargins(0, 0, 0, 0);
      setLayout(m_layout);
    }

    FocusNotifier *focusNotifier() const { return m_focusNotifier; }

    void setWrapped(QWidget *widget, FocusNotifier *focusNotifier = nullptr) {
      if (auto item = m_layout->takeAt(0); item && item->widget()) { item->widget()->deleteLater(); }

      m_layout->addWidget(widget);
      setFocusProxy(widget);

      m_focusNotifier = focusNotifier;
      m_widget = widget;
    }

    void dispatchRender(const std::shared_ptr<FormModel::IField> &field) {
      m_field = field;
      render(m_field);
    }

    virtual void render(const std::shared_ptr<FormModel::IField> &field) {}
  };

  class IField : public FieldBase {
  public:
    virtual void refresh(JsonFormField *widget) const {
      if (value) widget->setJsonValue(*value);
    }

    size_t fieldTypeId() const { return typeid(*this).hash_code(); }

    IField(const FieldBase &base) : FieldBase(base) { qDebug() << "onBlur" << onBlur << id; }
  };

  struct TextField : public IField {
    std::optional<QString> m_placeholder;

  public:
    void refresh(JsonFormField *widget) const override {}

    TextField(const FieldBase &base) : IField(base) {}
  };

  class JsonInputField : public JsonFormField {
    BaseInput *m_input = new BaseInput;
    std::shared_ptr<TextField> m_model;

  public:
    void handleTextChanged(const QString &text) {
      if (m_model->onChange) { m_extensionNotifier->notify(*m_model->onChange, text); }
    }

    void clear() override { m_input->setText(""); }

    QJsonValue jsonValue() const override { return m_input->text(); }

    void setJsonValue(const QJsonValue &value) const override { m_input->setText(value.toString()); }

    void render(const std::shared_ptr<IField> &field) override {
      m_model = std::static_pointer_cast<TextField>(field);

      if (auto placeholder = m_model->m_placeholder) m_input->setPlaceholderText(*placeholder);
      if (auto value = m_model->value) { m_input->setText(value->toString()); }
    }

    JsonInputField() {
      setWrapped(m_input, m_input->focusNotifier());
      connect(m_input, &BaseInput::textChanged, this, &JsonInputField::handleTextChanged);
    }
  };

  class CheckboxField : public IField {
  public:
    CheckboxField(const FieldBase &base) : IField(base) {}
  };

  class JsonCheckboxField : public JsonFormField {
    CheckboxInput *m_input = new CheckboxInput;
    std::shared_ptr<CheckboxField> m_model;

    void clear() override { return m_input->setValueAsJson(false); }

    void handleChange(bool value) {
      if (auto value = m_model->value) { m_input->setValueAsJson(*value); }
      if (auto change = m_model->onChange) { m_extensionNotifier->notify(*change, value); }
    }

    void setJsonValue(const QJsonValue &value) const override { m_input->setValueAsJson(value); }
    QJsonValue jsonValue() const override { return m_input->value(); }

  public:
    void render(const std::shared_ptr<FormModel::IField> &field) override {
      m_model = std::static_pointer_cast<CheckboxField>(field);

      if (auto value = m_model->value) { m_input->setValueAsJson(*value); }
    }

    JsonCheckboxField(QWidget *parent = nullptr) : JsonFormField(parent) {
      setWrapped(m_input);
      connect(m_input, &CheckboxInput::valueChanged, this, &JsonCheckboxField::handleChange);
    }
  };

  class DropdownSelectorItem : public SelectorInput::AbstractItem {
    DropdownModel::Item m_model;

    QString id() const override { return m_model.value; }

    OmniIconUrl icon() const override {
      return m_model.icon ? OmniIconUrl(*m_model.icon) : BuiltinOmniIconUrl("circle");
    }

    QString displayName() const override { return m_model.title; }

    AbstractItem *clone() const override { return new DropdownSelectorItem(*this); }

    const DropdownModel::Item &item() const { return m_model; }

  public:
    DropdownSelectorItem(const DropdownModel::Item &model) : m_model(model) {}
  };

  class DropdownField : public IField {

  public:
    DropdownField(const FieldBase &base) : IField(base) {}

    std::vector<DropdownModel::Child> m_items;
    std::optional<QString> onSearchTextChange;
    std::optional<QString> placeholder;
    bool isLoading;
    bool throttle;
    std::optional<QString> tooltip;

    void refresh(JsonFormField *widget) const override {}
  };

  class JsonDropdownField : public JsonFormField {
    SelectorInput *m_input = new SelectorInput;
    std::shared_ptr<DropdownField> m_model;

    void handleSelectionChanged(const SelectorInput::AbstractItem &item) {
      if (auto onChange = m_model->onChange) { m_extensionNotifier->notify(*onChange, item.id()); }
      if (auto value = m_model->value) { m_input->setValue(value->toString()); }
    }

    void clear() override { m_input->clear(); }

    void handleTextChanged(const QString &text) {
      if (auto change = m_model->onSearchTextChange) { m_extensionNotifier->notify(*change, text); }
      // XXX - implement text changed notification
      qDebug() << "handle text" << text;
    }

  public:
    QJsonValue jsonValue() const override {
      if (auto value = m_input->value()) { return value->id(); }
      return QJsonValue();
    }

    void setJsonValue(const QJsonValue &value) const override { m_input->setValue(value.toString()); }

    void render(const std::shared_ptr<FormModel::IField> &field) override {
      m_model = std::static_pointer_cast<DropdownField>(field);

      std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> items;

      for (const auto &item : m_model->m_items) {
        if (auto listItem = std::get_if<DropdownModel::Item>(&item)) {
          items.push_back(std::make_unique<DropdownSelectorItem>(*listItem));
        } else if (auto section = std::get_if<DropdownModel::Section>(&item)) {
          items.push_back(std::make_unique<OmniList::VirtualSection>(section->title));

          for (const auto &item : section->items) {
            qWarning() << "dropdown item" << item.value;
            items.push_back(std::make_unique<DropdownSelectorItem>(item));
          }
        }
      }

      m_input->list()->updateFromList(items);
      m_input->setIsLoading(m_model->isLoading);

      if (auto value = m_model->value) m_input->setValue(value->toString());
    }

    JsonDropdownField() {
      setWrapped(m_input, m_input->focusNotifier());
      connect(m_input, &SelectorInput::selectionChanged, this, &JsonDropdownField::handleSelectionChanged);
      connect(m_input, &SelectorInput::textChanged, this, &JsonDropdownField::handleTextChanged);
    }
  };

  struct TextFieldModel : public FieldBase {};
  struct PasswordFieldModel : public FieldBase {};
  struct TextAreaFieldModel : public FieldBase {
    bool enableMarkdown;
  };
  struct CheckboxFieldModel : public FieldBase {};
  struct DatePickerFieldModel : public FieldBase {};
  struct InvalidField : public FieldBase {};

  struct Separator {};
  struct Description {
    QString text;
    std::optional<QString> title;
  };

  using SearchAccessory = std::variant<DropdownModel>;
  using Field = std::variant<TextFieldModel, PasswordFieldModel, TextAreaFieldModel, CheckboxFieldModel,
                             DatePickerFieldModel, InvalidField>;
  using Item = std::variant<std::shared_ptr<IField>, Description, Separator>;

  bool isLoading;
  bool enableDrafts;
  std::optional<QString> navigationTitle;
  std::optional<SearchAccessory> accessory;
  std::optional<ActionPannelModel> actions;
  std::vector<Item> items;

  static FormModel fromJson(const QJsonObject &json);
};
