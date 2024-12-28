#pragma once
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
public:
  VDivider(size_t width = 1) {
    setFrameShape(QFrame::VLine);
    setFixedWidth(width);
    setProperty("class", "divider");
  }
};

class SplitView : public QWidget {
  QHBoxLayout *layout = nullptr;
  QWidget *right = nullptr;
  VDivider *divider = nullptr;

public:
  SplitView(QWidget *main, QWidget *right)
      : right(right), divider(new VDivider) {
    layout = new QHBoxLayout();
    layout->addWidget(main, 1);
    layout->addWidget(divider);
    layout->addWidget(right, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);
  }

  void collapse() {
    if (right) {
      divider->hide();
      right->hide();
    }
  }

  void expand() {
    if (right) {
      divider->show();
      right->show();
    }
  }
};

class IconLabel {};

class Container : public QWidget {
public:
  Container(QWidget *child, size_t maxWidth = 600) {
    auto box = new QVBoxLayout();

    box->addWidget(child, 0, Qt::AlignHCenter);
    // box->addWidget(child, 0);

    setLayout(box);
  }
};

class IInputHandler {
public:
  virtual void handleInput(QKeyEvent *event) = 0;
};
