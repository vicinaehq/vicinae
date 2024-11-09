#pragma once
#include "render.hpp"
#include <jsoncpp/json/value.h>
#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AppWindow : public QMainWindow {
  Q_OBJECT

  Component *root = nullptr;

public:
  AppWindow(QWidget *parent = 0);

signals:
  void render(Json::Value &root);
};
