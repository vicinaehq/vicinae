#pragma once
#include "theme.hpp"
#include <QHBoxLayout>
#include <QString>
#include <cmath>
#include <functional>
#include <optional>
#include <qboxlayout.h>
#include <qevent.h>
#include <qframe.h>
#include <qicon.h>
#include <qjsonvalue.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qprocess.h>
#include <qwidget.h>
#include <qwindowdefs.h>

template <typename T> struct PaginatedResponse {
  int totalCount;
  int currentPage;
  int totalPages;
  std::vector<T> data;
};

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
  QColor _color;
  size_t _height;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);
    auto margins = contentsMargins();
    auto &theme = ThemeService::instance().theme();

    painter.setBrush(QBrush(theme.colors.border));
    painter.setPen(theme.colors.border);
    painter.drawRect(0, margins.top(), width(), _height);
  }

public:
  void setHeight(int height) {
    _height = height;
    setFixedHeight(height);
    updateGeometry();
  }

  void setColor(QColor color) {
    _color = color;
    update();
  }

  HDivider(QWidget *parent = nullptr) : _height(1), _color("#222222") { setFixedHeight(_height); }
};

class VDivider : public QFrame {
  QColor _color;
  size_t _width;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);
    auto margins = contentsMargins();
    auto &theme = ThemeService::instance().theme();

    painter.setBrush(QBrush(theme.colors.border));
    painter.setPen(theme.colors.border);
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

  VDivider(QWidget *parent = nullptr) : QFrame(parent), _width(1), _color("#222222") {
    setFixedWidth(_width);
  }
};

struct IJsonFormField {
  virtual QJsonValue asJsonValue() const = 0;
  virtual void setValueAsJson(const QJsonValue &value) = 0;
};

struct LaunchProps {
  std::vector<std::pair<QString, QString>> arguments;
};
