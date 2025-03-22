#pragma once
#include "ui/action-pannel/action.hpp"
#include <qevent.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ActionPannelView : public QWidget {
  Q_OBJECT

public:
  virtual void onSearchChanged(const QString &s) {};
  virtual std::vector<AbstractAction *> actions() const = 0;

signals:
  void pushView(ActionPannelView *view) const;
  void actionActivated(AbstractAction *action) const;
};
