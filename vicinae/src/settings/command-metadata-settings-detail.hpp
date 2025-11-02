#pragma once
#include "common.hpp"
#include "preference.hpp"
#include "theme.hpp"
#include "ui/file-picker/file-picker.hpp"
#include "ui/preference-dropdown/preference-dropdown.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/form.hpp"
#include "ui/form/password-input.hpp"
#include "ui/switch/switch.hpp"
#include "ui/typography/typography.hpp"
#include "utils/layout.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <unistd.h>

class AbstractPreferenceFormItem : public QWidget {
public:
  AbstractPreferenceFormItem(QWidget *parent = nullptr) : QWidget(parent) {}

  virtual JsonFormItemWidget *formItem() const = 0;
};

class PreferenceSwitchFormItem : public AbstractPreferenceFormItem {
  TypographyWidget *m_label = new TypographyWidget;
  TypographyWidget *m_description = new TypographyWidget;
  Switch *m_switch = new Switch;

  void setupUI() {
    m_description->setWordWrap(true);
    m_description->setSize(TextSize::TextSmaller);
    m_label->setColor(SemanticColor::TextMuted);

    VStack()
        .spacing(10)
        .add(HStack().add(m_label).add(m_switch).justifyBetween())
        .add(m_description)
        .imbue(this);
  }

  JsonFormItemWidget *formItem() const override { return m_switch; }

public:
  void setLabel(const QString &title) { m_label->setText(title); }
  void setDescription(const QString &title) { m_description->setText(title); }

  PreferenceSwitchFormItem(QWidget *parent = nullptr) : AbstractPreferenceFormItem(parent) { setupUI(); }
};

class FilePickerPreferenceFormItem : public AbstractPreferenceFormItem {
  QVBoxLayout *m_vlayout = new QVBoxLayout;
  FilePicker *m_filePicker = new FilePicker(this);
  TypographyWidget *m_label = new TypographyWidget;
  TypographyWidget *m_description = new TypographyWidget;

  void setupUI() {
    m_label->setColor(SemanticColor::TextMuted);
    m_description->setWordWrap(true);
    m_description->setSize(TextSize::TextSmaller);
    m_vlayout->setContentsMargins(0, 0, 0, 0);
    m_vlayout->setSpacing(10);
    m_vlayout->addWidget(m_label);
    m_vlayout->addWidget(m_description);
    m_vlayout->addWidget(m_filePicker);
    setLayout(m_vlayout);
  }

  JsonFormItemWidget *formItem() const override { return m_filePicker; }

public:
  void setLabel(const QString &title) { m_label->setText(title); }
  void setDescription(const QString &title) { m_description->setText(title); }
  FilePicker *filerPicker() const { return m_filePicker; }

  FilePickerPreferenceFormItem(QWidget *parent = nullptr) : AbstractPreferenceFormItem(parent) { setupUI(); }
};

class VerticalFormItem : public AbstractPreferenceFormItem {
  QVBoxLayout *m_vlayout = new QVBoxLayout;
  TypographyWidget *m_label = new TypographyWidget;
  TypographyWidget *m_description = new TypographyWidget;
  QWidget *m_content = new QWidget;
  JsonFormItemWidget *m_item = nullptr;

  void setupUI() {
    m_label->setColor(SemanticColor::TextMuted);
    m_description->setWordWrap(true);
    m_description->setSize(TextSize::TextSmaller);
    m_vlayout->setContentsMargins(0, 0, 0, 0);
    m_vlayout->setSpacing(10);
    m_vlayout->addWidget(m_label);
    m_vlayout->addWidget(m_content);
    m_vlayout->addWidget(m_description);
    setLayout(m_vlayout);
  }

  JsonFormItemWidget *formItem() const override { return m_item; }

public:
  void setLabel(const QString &title) { m_label->setText(title); }
  void setDescription(const QString &title) { m_description->setText(title); }

  void setContent(JsonFormItemWidget *widget) {
    if (auto item = m_vlayout->itemAt(1)) {
      if (auto previous = item->widget()) {
        m_vlayout->replaceWidget(previous, widget);
        previous->deleteLater();
      }
    }
    m_item = widget;
  }

  VerticalFormItem(QWidget *parent = nullptr) : AbstractPreferenceFormItem(parent) { setupUI(); }
};

struct PreferenceWidgetVisitor {
  Preference m_preference;

  AbstractPreferenceFormItem *operator()(const auto &data) {
    auto item = new VerticalFormItem;
    item->setLabel(m_preference.title());
    item->setContent(new BaseInput);
    item->setDescription(m_preference.description());
    return item;
  }

  AbstractPreferenceFormItem *operator()(const Preference::FilePickerData &data) {
    auto item = new VerticalFormItem;
    auto picker = new FilePicker;
    picker->setMultiple(data.multiple);
    item->setLabel(m_preference.title());
    if (m_preference.isReadOnly()) { picker->setReadOnly(); }
    item->setContent(picker);
    item->setDescription(m_preference.description());
    return item;
  }

  AbstractPreferenceFormItem *operator()(const Preference::DirectoryPickerData &data) {
    auto item = new VerticalFormItem;
    auto picker = new FilePicker;
    picker->setMultiple(data.multiple);
    if (m_preference.isReadOnly()) { picker->setReadOnly(); }
    picker->setOnlyDirectories();
    item->setLabel(m_preference.title());
    item->setContent(picker);
    item->setDescription(m_preference.description());
    return item;
  }

  AbstractPreferenceFormItem *operator()(const Preference::TextData &text) {
    auto item = new VerticalFormItem;
    auto input = new BaseInput;
    input->setPlaceholderText(m_preference.placeholder());
    item->setLabel(m_preference.title());
    item->setContent(input);
    item->setDescription(m_preference.description());
    return item;
  }
  AbstractPreferenceFormItem *operator()(const Preference::PasswordData &password) {
    auto item = new VerticalFormItem;
    auto input = new PasswordInput;
    input->setPlaceholderText(m_preference.placeholder());
    item->setLabel(m_preference.title());
    item->setContent(input);
    item->setDescription(m_preference.description());
    return item;
  }
  AbstractPreferenceFormItem *operator()(const Preference::DropdownData &data) {
    auto item = new VerticalFormItem;
    auto selector = new PreferenceDropdown;

    item->setLabel(m_preference.title());
    item->setContent(selector);
    item->setDescription(m_preference.description());
    selector->setOptions(data.options);

    return item;
  }
  AbstractPreferenceFormItem *operator()(const Preference::CheckboxData &data) {
    auto item = new PreferenceSwitchFormItem;

    item->setLabel(data.label.value_or(m_preference.title()));
    item->setDescription(m_preference.description());

    return item;
  }
  AbstractPreferenceFormItem *operator()(const Preference::UnknownData &data) { return nullptr; }

public:
  PreferenceWidgetVisitor(const Preference &preference) : m_preference(preference) {}
};

class CommandMetadataSettingsDetailWidget : public QWidget {
  QString m_rootItemId;
  std::shared_ptr<AbstractCmd> m_command;
  QJsonObject m_preferenceValues;
  FormWidget *m_form = new FormWidget;
  std::map<QString, AbstractPreferenceFormItem *> m_preferenceFields;
  QVBoxLayout *m_layout = new QVBoxLayout;

  void setupUI();
  void savePendingPreferences();

public:
  CommandMetadataSettingsDetailWidget(const QString &rootItemId, const std::shared_ptr<AbstractCmd> &cmd);
  ~CommandMetadataSettingsDetailWidget();
};
