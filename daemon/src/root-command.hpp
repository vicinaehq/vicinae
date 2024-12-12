#pragma once

#include "app.hpp"
#include "command.hpp"
#include <qlabel.h>
#include <qwidget.h>

class RootView : public View {
public:
  virtual void onSearchChanged(const QString &s) override {
    qDebug() << "search changed " << s;
  }

  RootView(AppWindow &app) : View(app) { widget = new QLabel("root"); }
};

class RootCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new RootView(app); }
};
