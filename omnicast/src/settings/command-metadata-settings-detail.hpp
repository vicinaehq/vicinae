#pragma once
#include "common.hpp"
#include "preference.hpp"
#include "theme.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/form.hpp"
#include "ui/form/password-input.hpp"
#include "ui/typography.hpp"
#include <libqalculate/Number.h>
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qwidget.h>
#include <unistd.h>

class AbstractPreferenceFormItem : public QWidget {
public:
  AbstractPreferenceFormItem(QWidget *parent = nullptr) : QWidget(parent) {}

  virtual JsonFormItemWidget *formItem() const = 0;
};

class VerticalFormItem : public AbstractPreferenceFormItem {
  QVBoxLayout *m_vlayout = new QVBoxLayout;
  TypographyWidget *m_label = new TypographyWidget;
  TypographyWidget *m_description = new TypographyWidget;
  QWidget *m_content = new QWidget;
  JsonFormItemWidget *m_item = nullptr;

  void setupUI() {
    m_label->setColor(ColorTint::TextSecondary);
    m_description->setColor(ColorTint::TextSecondary);
    m_description->setWordWrap(true);
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

  AbstractPreferenceFormItem *operator()(const Preference::TextData &text) {
    auto item = new VerticalFormItem;
    item->setLabel(m_preference.title());
    item->setContent(new BaseInput);
    item->setDescription(m_preference.description());
    return item;
  }
  AbstractPreferenceFormItem *operator()(const Preference::PasswordData &password) {
    auto item = new VerticalFormItem;
    item->setLabel(m_preference.title());
    item->setContent(new PasswordInput);
    item->setDescription(m_preference.description());
    return item;
  }
  AbstractPreferenceFormItem *operator()(const Preference::DropdownData &data) { return nullptr; }
  AbstractPreferenceFormItem *operator()(const Preference::CheckboxData &data) { return nullptr; }
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
};
