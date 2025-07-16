#pragma once
#include "ui/action-pannel/action.hpp"
#include <qobject.h>

class SetThemeAction : public AbstractAction {
  QString m_themeId;

  void execute(AppWindow &app) override {}
  void execute() override;
  void execute(ApplicationContext *context) override;

public:
  SetThemeAction(const QString &themeId);
};
