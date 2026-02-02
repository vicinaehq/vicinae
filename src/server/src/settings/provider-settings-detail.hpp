#pragma once
#include <qtimer.h>
#include <qwidget.h>
#include "settings/command-metadata-settings-detail.hpp"

class RootProvider;

class ProviderSettingsDetail : public QWidget {
public:
  ProviderSettingsDetail(const RootProvider &provider);
  ~ProviderSettingsDetail();

private:
  void setupUI(const RootProvider &provider);
  void savePendingPreferences();
  void handleFocusChanged(bool focused);

  QString m_rootItemId;
  QJsonObject m_preferenceValues;
  std::map<QString, AbstractPreferenceFormItem *> m_preferenceFields;
  QVBoxLayout *m_layout = new QVBoxLayout;
  QTimer m_timer;
};
