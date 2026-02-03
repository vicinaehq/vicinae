#pragma once
#include "config/config.hpp"
#include "theme.hpp"
#include <qobject.h>

class ThemeListModel;

class ThemeListController : public QObject {
  Q_OBJECT

public:
  ThemeListController(config::Manager *configService, ThemeService *themeService, ThemeListModel *model,
                      QObject *parent = nullptr);

  void setFilter(const QString &query);
  void regenerateThemes();

private slots:
  void handleConfigChanged();

private:
  ThemeListModel *m_model = nullptr;
  config::Manager *m_configService = nullptr;
  ThemeService *m_themeService = nullptr;
  QString m_query;
};
