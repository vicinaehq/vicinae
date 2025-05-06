#pragma once
#include "ui/action-pannel/action.hpp"
#include <memory>
#include <qevent.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ActionPannelView : public QWidget {
  Q_OBJECT

public:
  virtual void onSearchChanged(const QString &s) {};
  virtual std::vector<std::shared_ptr<AbstractAction>> actions() const = 0;

signals:
  void pushView(ActionPannelView *view) const;
  void actionActivated(AbstractAction *action) const;
};
