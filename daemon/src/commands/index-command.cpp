#include "index-command.hpp"
#include "command-database.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
#include "tinyexpr.hpp"
#include "xdg-desktop-database.hpp"
#include <cmath>
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

IndexCommand::IndexCommand(AppWindow *app)
    : CommandWidget(app), list(new ManagedList()) {

  xdg = new XdgDesktopDatabase();
  cmdDb = new CommandDatabase();
  quicklinkDb = new QuicklistDatabase();

  for (const auto &cmd : cmdDb->commands) {
    if (!cmd.usableWith)
      continue;
    usableWithCommands.push_back(new Command(cmd));
  }

  auto layout = new QVBoxLayout();

  app->topBar->input->setPlaceholderText("Search for apps or commands...");
  // app->topBar->input->setTextMargins(15, 20, 0, 20);

  list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list->setFocusPolicy(Qt::NoFocus);
  list->setSpacing(0);

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setAlignment(Qt::AlignTop);

  layout->addWidget(list, 1);

  app->topBar->input->installEventFilter(this);

  connect(list, &ManagedList::itemSelected, this, &IndexCommand::itemSelected);
  connect(list, &ManagedList::itemActivated, this,
          &IndexCommand::itemActivated);

  onSearchChanged("");

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

    if (keyEvent->key() == Qt::Key_Return && app->topBar->quickInput) {
      if (app->topBar->quickInput->focusFirstEmpty())
        return true;
    }

    if (isListKey(keyEvent)) {
      QApplication::sendEvent(list, event);
      return true;
    }
  }

  return false;
}

void IndexCommand::itemSelected(const IActionnable &item) {
  destroyCompletion();

  if (auto quicklink = dynamic_cast<const Quicklink *>(&item)) {
    createCompletion(quicklink->placeholders);
    app->topBar->quickInput->setIcon(quicklink->iconName);

    for (const auto &input : app->topBar->quickInput->inputs) {
      input->installEventFilter(this);
    }
  }

  auto actions = item.generateActions();

  if (!actions.isEmpty()) {
    app->statusBar->setSelectedAction(actions.at(0));
  } else {
  }

  qDebug() << "Selection changed";
}

void IndexCommand::itemActivated(const IActionnable &item) {
  auto actions = item.generateActions();

  if (actions.isEmpty()) {
    return;
  }

  if (auto command = dynamic_cast<const Command *>(&item)) {
    app->setCommand(command);
    return;
  }

  QList<QString> command{app->topBar->input->text()};

  if (auto completer = app->topBar->quickInput) {
    for (const auto &input : completer->inputs) {
      command.push_back(input->text());
    }
  }

  actions.at(0)->exec(command);
}

void IndexCommand::onSearchChanged(const QString &text) {
  list->clear();

  std::string_view query(text.toLatin1().data());

  if (QColor(text).isValid()) {
    list->addSection("Color");

    auto circle = new ColorCircle(text, QSize(60, 60));
    auto colorLabel = new QLabel(text);

    colorLabel->setProperty("class", "transform-left");

    auto left = new VStack(colorLabel, new Chip("HEX"));
    auto right = new VStack(circle, new Chip(text));
    auto widget = new TransformResult(left, right);

    list->addWidgetItem(new CodeToColor(text), widget);
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

      list->addWidgetItem(new Calculator(text, answerLabel->text()),
                          new TransformResult(left, right));
    }
  }

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

      list->addWidgetItem(new Quicklink(quicklink), widget);
    }
  }

  for (size_t i = 0; i != apps.size(); ++i) {
    auto &app = apps.at(i);
    auto widget = new GenericListItem(QString::fromStdString(app.icon),
                                      QString::fromStdString(app.name), "",
                                      "Application");

    list->addWidgetItem(new App(app), widget);
  }

  for (const auto &cmd : matchingCommands) {
    auto widget =
        new GenericListItem(cmd.iconName, cmd.name, cmd.category, "Command");

    list->addWidgetItem(new Command(cmd), widget);
  }

  if (usableWithCommands.size() > 0) {
    list->addSection(QString("Use \"%1\" with...").arg(text));
  }

  for (const auto cmd : usableWithCommands) {
    auto widget =
        new GenericListItem(cmd->iconName, cmd->name, cmd->category, "Command");

    list->addWidgetItem(new Command(*cmd), widget);
  }

  if (list->count() == 0)
    destroyCompletion();

  // select first selectable element
  for (int i = 0; i != list->count(); ++i) {
    auto item = list->item(i);

    if (!item->flags().testFlag(Qt::ItemIsSelectable))
      continue;

    list->setCurrentItem(item);
    break;
  }
}
