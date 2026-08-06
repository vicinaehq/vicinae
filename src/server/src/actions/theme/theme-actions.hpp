#pragma once
#include <QCoreApplication>
#include "ui/action-pannel/action.hpp"
#include <qobject.h>

class SetThemeAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(SetThemeAction)

  QString m_themeId;

  void execute(ApplicationContext *context) override;

public:
  SetThemeAction(const QString &themeId);
};
