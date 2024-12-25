#pragma once
#include "view.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qthread.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AppWindow;

class Command : public QObject {};

class ViewCommand : public Command {
public:
  ViewCommand() {}

  virtual View *load(AppWindow &) = 0;
  virtual void unload(AppWindow &) {}

  ~ViewCommand() { qDebug() << "destroyed view"; }
};

class HeadlessCommand : public Command {
  virtual void load() = 0;
};
