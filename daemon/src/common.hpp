#pragma once
#include <QHBoxLayout>
#include <QString>
#include <memory>
#include <qboxlayout.h>
#include <qframe.h>
#include <qicon.h>
#include <qlabel.h>
#include <qprocess.h>
#include <qwidget.h>

class ActionExecutionContext;

class IAction {
public:
  virtual QString name() const = 0;
  virtual QIcon icon() const {
    return QIcon::fromTheme("application-x-executable");
  }
};

class IActionnable {
protected:
  using ActionList = QList<std::shared_ptr<IAction>>;

public:
  virtual ActionList generateActions() const = 0;
};

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
