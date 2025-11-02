#pragma once
#include "settings/command-metadata-settings-detail.hpp"
#include <qwidget.h>

class RootProvider;

class ProviderSettingsDetail : public QWidget {
  QString m_rootItemId;
  QJsonObject m_preferenceValues;
  std::map<QString, AbstractPreferenceFormItem *> m_preferenceFields;
  QVBoxLayout *m_layout = new QVBoxLayout;

  void setupUI(const RootProvider &provider);
  void savePendingPreferences();
  void handleFocusChanged(bool focused);

public:
  ProviderSettingsDetail(const RootProvider &provider);
  ~ProviderSettingsDetail();
};
