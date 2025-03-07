#pragma once
#include "ui/action_popover.hpp"
#include <QHBoxLayout>
#include <QString>
#include <cmath>
#include <functional>
#include <optional>
#include <qboxlayout.h>
#include <qevent.h>
#include <qframe.h>
#include <qicon.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qprocess.h>
#include <qwidget.h>
#include <qwindowdefs.h>

class NonAssignable {
public:
  NonAssignable(const NonAssignable &) = delete;
  NonAssignable &operator=(const NonAssignable &) = delete;
  NonAssignable() {}
};

template <class T> using OptionalRef = std::optional<std::reference_wrapper<T>>;

class CommandObject;
class ExecutionContext;

static void xdgOpen(const QString &url) {
  QProcess process;

  process.startDetached("xdg-open", QStringList() << url);
}

class CommandObject;
class AppWindow;

class ICommandFactory {
public:
  virtual CommandObject *operator()(AppWindow *app) = 0;
};

template <typename T> class BasicCommandFactory : public ICommandFactory {
public:
  CommandObject *operator()(AppWindow *app) { return new T(app); }
};

class HDivider : public QFrame {
public:
  HDivider(size_t height = 1) {
    setFrameShape(QFrame::HLine);
    setFixedHeight(height);
    setProperty("class", "divider");
  }
};

class VDivider : public QFrame {
  QColor _color;
  size_t _width;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);
    auto margins = contentsMargins();

    painter.setBrush(QBrush(_color));
    painter.setPen(_color);
    painter.drawRect(0, margins.top(), _width, height() - margins.top() - margins.bottom());
  }

public:
  void setWidth(int width) {
    _width = width;
    setFixedWidth(width);
    updateGeometry();
  }

  void setColor(QColor color) {
    _color = color;
    update();
  }

  VDivider(QWidget *parent = nullptr) : _width(1), _color("#222222") { setFixedWidth(_width); }
};
