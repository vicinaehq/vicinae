#include "index-command.hpp"
#include "command-database.hpp"
#include "quicklist-database.hpp"
#include "tinyexpr.hpp"
#include "xdg-desktop-database.hpp"
#include <cmath>
#include <memory>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qcoreevent.h>
#include <qdebug.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qwidget.h>
#include <string_view>
#include <variant>

IndexCommand::IndexCommand(QWidget *parent)
    : QWidget(parent), topBar(new TopBar()), list(new ManagedList()),
      statusBar(new StatusBar()) {

  xdg = std::make_unique<XdgDesktopDatabase>(XdgDesktopDatabase());
  cmdDb = std::make_unique<CommandDatabase>(CommandDatabase());
  quicklinkDb = std::make_unique<QuicklistDatabase>(QuicklistDatabase());

  for (const auto &cmd : cmdDb->commands) {
    if (!cmd.usableWith)
      continue;
    usableWithCommands.push_back(cmd);
  }

  auto layout = new QVBoxLayout();

  topBar->input->setPlaceholderText("Search for apps or commands...");
  topBar->input->setTextMargins(15, 20, 0, 20);

  list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list->setFocusPolicy(Qt::NoFocus);
  list->setSpacing(0);

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setAlignment(Qt::AlignTop);

  layout->addWidget(topBar);
  layout->addWidget(list, 1);
  layout->addWidget(statusBar);

  topBar->input->installEventFilter(this);

  connect(topBar->input, &QLineEdit::textChanged, this,
          &IndexCommand::inputTextChanged);
  connect(list, &ManagedList::itemSelected, this, &IndexCommand::itemSelected);
  connect(list, &ManagedList::itemActivated, this,
          &IndexCommand::itemActivated);

  setLayout(layout);
}

static QListWidgetItem *generateLabel(const QString &name, size_t idx) {
  auto item = new QListWidgetItem();
  auto widget = new QLabel("Results");

  widget->setContentsMargins(8, idx > 0 ? 10 : 0, 0, 10);
  item->setFlags(item->flags() & !Qt::ItemIsSelectable);
  widget->setProperty("class", "minor category-name");

  return item;
}

static bool isListKey(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Left:
  case Qt::Key_Right:
  case Qt::Key_Up:
  case Qt::Key_Down:
  case Qt::Key_Return:
    return true;
  default:
    return false;
  }
}

bool IndexCommand::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);

    qDebug() << QKeySequence(keyEvent->key()).toString();

    if (keyEvent->key() == Qt::Key_Return && topBar->quickInput) {
      if (topBar->quickInput->focusFirstEmpty())
        return true;
    }

    if (isListKey(keyEvent)) {
      QApplication::sendEvent(list, event);
      return true;
    }
  }

  return false;
}

void IndexCommand::itemSelected(const Selectable &item) {
  topBar->destroyQuicklinkCompleter();

  if (auto command = std::get_if<Command>(&item)) {
    qDebug() << "command " << command->name;
  } else if (auto app = std::get_if<App>(&item)) {
    qDebug() << "app " << app->name;
  } else if (auto quicklink = std::get_if<Quicklink>(&item)) {
    if (!quicklink->placeholders.isEmpty()) {
      topBar->activateQuicklinkCompleter(*quicklink);
      for (const auto &input : topBar->quickInput->inputs) {
        input->installEventFilter(this);
      }
    }

    qDebug() << "quicklink " << quicklink->name;
  }
}

void IndexCommand::itemActivated(const Selectable &item) {
  if (auto command = std::get_if<Command>(&item)) {
    qDebug() << "activated command " << command->name;
  } else if (auto app = std::get_if<App>(&item)) {
    qDebug() << "activated app " << app->name;
  } else if (auto quicklink = std::get_if<Quicklink>(&item)) {
    if (auto completer = topBar->quickInput) {
      for (const auto &arg : completer->collectArgs()) {
        qDebug() << "arg=" << arg;
      }
    }
    qDebug() << "activated quicklink " << quicklink->name;
  }
}

void IndexCommand::inputTextChanged(const QString &text) {
  list->clear();
  selectables.clear();

  std::string_view query(text.toLatin1().data());

  /*
  if (QColor(text).isValid()) {
    list->addSection("Color");

    auto circle = new ColorCircle(text, QSize(60, 60));
    auto colorLabel = new QLabel(text);

    colorLabel->setProperty("class", "transform-left");

    auto left = new VStack(colorLabel, new Chip("HEX"));
    auto right = new VStack(circle, new Chip(text));
    auto widget = new TransformResult(left, right);

    list->addWidgetItem(widget, nullptr);
  }

  if (text.size() > 1) {
    te_parser parser;

    if (double result = parser.evaluate(query); !std::isnan(result)) {
      list->addSection("Calculator");

      auto exprLabel = new QLabel(text);

      exprLabel->setProperty("class", "transform-left");

      auto answerLabel = new QLabel(QString::number(result));
      answerLabel->setProperty("class", "transform-left");

      auto left = new VStack(exprLabel, new Chip("Expression"));
      auto right = new VStack(answerLabel, new Chip("Answer"));

      list->addWidgetItem(new TransformResult(left, right));
    }
  }
  */

  auto apps = xdg->query(text.toStdString());
  QList<Command> matchingCommands;

  for (const auto &cmd : cmdDb->commands) {
    if (!cmd.normalizedName.contains(text))
      continue;

    matchingCommands.push_back(cmd);
  }

  if (!apps.empty() || !matchingCommands.empty()) {
    list->addSection("Results");
  }

  for (const auto &quicklink : quicklinkDb->links) {
    if (text.size() > 0 && quicklink.name.startsWith(text)) {
      qDebug() << "quicklink matches " << quicklink.displayName;

      auto widget = new GenericListItem(quicklink.iconName,
                                        quicklink.displayName, "", "Quicklink");

      list->addWidgetItem(quicklink, widget);
    }
  }

  for (size_t i = 0; i != apps.size(); ++i) {
    auto &app = apps.at(i);
    auto widget = new GenericListItem(QString::fromStdString(app.icon),
                                      QString::fromStdString(app.name), "",
                                      "Application");

    list->addWidgetItem(app, widget);
  }

  for (const auto &cmd : matchingCommands) {
    auto widget =
        new GenericListItem(cmd.iconName, cmd.name, cmd.category, "Command");

    selectables.push_back(cmd);
    list->addWidgetItem(cmd, widget);
  }

  if (usableWithCommands.size() > 0) {
    list->addSection(QString("Use \"%1\" with...").arg(text));
  }

  for (const auto &cmd : usableWithCommands) {
    auto widget =
        new GenericListItem(cmd.iconName, cmd.name, cmd.category, "Command");

    selectables.push_back(cmd);
    list->addWidgetItem(cmd, widget);
  }

  // select first selectable element
  for (int i = 0; i != list->count(); ++i) {
    auto item = list->item(i);

    if (!item->flags().testFlag(Qt::ItemIsSelectable))
      continue;

    list->setCurrentItem(item);
    break;
  }
}
