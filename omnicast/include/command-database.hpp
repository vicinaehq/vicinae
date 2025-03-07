#pragma once
#include <QKeyEvent>
#include <QString>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qdir.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlogging.h>
#include <qwidget.h>

class AppWindow;
class ViewCommand;

struct BuiltinCommand {
  QString id;
  QString name;
  QString iconName;
  std::function<ViewCommand *(AppWindow &app, const QString &)> factory;
};

class CommandDatabase {
public:
  const std::vector<BuiltinCommand> &list();
  const BuiltinCommand *findById(const QString &id);

  CommandDatabase() {}
};
