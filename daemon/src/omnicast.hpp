#pragma once
#include "calculator-database.hpp"
#include "common.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/status_bar.hpp"
#include "ui/top_bar.hpp"
#include "xdg-desktop-database.hpp"
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qmath.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qprocess.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <stack>

class GenericListItem : public QWidget {
  QLabel *iconLabel;
  QLabel *name;
  QLabel *category;
  QLabel *kind;

public:
  GenericListItem(const QString &iconName, const QString &name,
                  const QString &category, const QString &kind,
                  QWidget *parent = nullptr)
      : QWidget(parent), iconLabel(new QLabel), name(new QLabel),
        category(new QLabel), kind(new QLabel) {

    auto mainLayout = new QHBoxLayout();

    setLayout(mainLayout);

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    auto icon = QIcon::fromTheme(iconName);

    if (icon.isNull())
      icon = QIcon::fromTheme("desktop");

    this->iconLabel->setPixmap(icon.pixmap(25, 25));

    this->name->setText(name);
    this->category->setText(category);
    this->category->setProperty("class", "minor");

    left->setLayout(leftLayout);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(this->iconLabel);
    leftLayout->addWidget(this->name);
    leftLayout->addWidget(this->category);

    mainLayout->addWidget(left, 0, Qt::AlignLeft);

    this->kind->setText(kind);
    this->kind->setProperty("class", "minor");
    mainLayout->addWidget(this->kind, 0, Qt::AlignRight);
  }
};

class VStack : public QWidget {
public:
  VStack(QWidget *left, QWidget *right) {
    auto layout = new QVBoxLayout();

    layout->setSpacing(10);
    layout->addWidget(left, 1, Qt::AlignCenter);
    layout->addWidget(right, 1, Qt::AlignBottom | Qt::AlignCenter);
    setLayout(layout);
  }
};

class Chip : public QLabel {
public:
  Chip(const QString &s) {
    setText(s);
    setContentsMargins(10, 5, 10, 5);
    setProperty("class", "chip");
  }
};

class TransformResult : public QWidget {

protected:
  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    int w = width();
    int h = height();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#666666"));
    painter.drawRect(w / 2, 0, 1, h / 2 - 20);
    painter.drawRect(w / 2, h / 2 + 20, 1, h / 2 - 20);
  }

public:
  TransformResult(QWidget *posLeft, QWidget *posRight) {
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(posLeft, 1);
    layout->addWidget(posRight, 1);
    setLayout(layout);
  }
};

class Command;
class CommandObject;
class AppDatabase;

template <class T> using Service = std::shared_ptr<T>;

class AppWindow : public QMainWindow {
  Q_OBJECT

public:
  std::stack<CommandObject *> commandStack;
  std::stack<QString> queryStack;

  std::shared_ptr<QuicklistDatabase> quicklinkDatabase;
  std::shared_ptr<XdgDesktopDatabase> xdd;
  std::shared_ptr<CalculatorDatabase> calculatorDatabase;
  Service<AppDatabase> appDb;

  template <typename T> std::shared_ptr<T> service() const;

  TopBar *topBar = nullptr;
  StatusBar *statusBar = nullptr;
  ActionPopover *actionPopover;

  // CommandObject *command = nullptr;
  QVBoxLayout *layout = nullptr;
  std::optional<const Command *> currentCommand = std::nullopt;

  AppWindow(QWidget *parent = 0);

  void resetCommand();
  void setCommandObject(CommandObject *cmd);

public slots:
  void pushCommandObject(std::shared_ptr<ICommandFactory> factory);
  void popCommandObject();

  void setCommand(const Command *command);
  bool eventFilter(QObject *obj, QEvent *event) override;
};
