#pragma once
#include "common.hpp"
#include "ui/form/form.hpp"
#include "ui/form/preference-field.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qwidget.h>
#include <unistd.h>

class CommandMetadataSettingsDetailWidget : public QWidget {
  QString m_rootItemId;
  std::shared_ptr<AbstractCmd> m_command;
  QJsonObject m_preferenceValues;
  FormWidget *m_form = new FormWidget;
  std::vector<PreferenceField *> m_preferenceFields;
  QVBoxLayout *m_layout = new QVBoxLayout;

  void setupUI();

public:
  CommandMetadataSettingsDetailWidget(const QString &rootItemId, const std::shared_ptr<AbstractCmd> &cmd);
};
