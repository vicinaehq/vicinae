#include "commands/index/index-command.hpp"
#include "calculator-database.hpp"
#include "calculator.hpp"
#include "command-database.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
#include "ui/color_circle.hpp"
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
#include <qwindowdefs.h>
#include <string_view>

IndexCommand::IndexCommand() : CommandObject(), list(new ManagedList()) {

  xdg = new XdgDesktopDatabase();
  cmdDb = new CommandDatabase();
  quicklinkDb = new QuicklistDatabase();

  for (const auto &cmd : cmdDb->commands) {
    if (!cmd.usableWith)
      continue;
    usableWithCommands.push_back(new Command(cmd));
  }

  auto layout = new QVBoxLayout();

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setAlignment(Qt::AlignTop);

  layout->addWidget(list, 1);

  connect(list, &ManagedList::itemSelected, this, &IndexCommand::itemSelected);
  connect(list, &ManagedList::itemActivated, this,
          &IndexCommand::itemActivated);

  forwardInputEvents(list);

  widget->setLayout(layout);
}

void IndexCommand::onMount() { onSearchChanged(""); }

void IndexCommand::onAttach() {
  setSearchPlaceholder("Search apps and commands...");
  setSearch(query);
  searchbar()->selectAll();
}

void IndexCommand::itemSelected(const IActionnable &item) {
  destroyCompletion();

  if (auto link = dynamic_cast<const Quicklink *>(&item)) {
    createCompletion(link->placeholders, link->iconName);
  }

  setActions(item.generateActions());
  qDebug() << "Selection changed";
}

void IndexCommand::itemActivated(const IActionnable &item) {
  auto actions = item.generateActions();

  if (actions.isEmpty())
    return;

  onActionActivated(actions.at(0));
}

void IndexCommand::onActionActivated(std::shared_ptr<IAction> action) {
  if (auto openApp = std::dynamic_pointer_cast<App::Open>(action)) {
    qDebug() << "opening app " << openApp->name();
  }

  if (auto execCmd =
          std::dynamic_pointer_cast<Command::ExecuteCommand>(action)) {
    pushCommand(execCmd->ref.widgetFactory());
    return;
  }

  if (auto ac = std::dynamic_pointer_cast<Calculator::CopyAction>(action)) {
    CalculatorDatabase::get().saveComputation(ac->ref.expression,
                                              ac->ref.result);
    setToast("Copied in clipboard");
    return;
  }

  if (auto ac = std::dynamic_pointer_cast<Calculator::OpenCalculatorHistory>(
          action)) {
    CalculatorDatabase::get().saveComputation(ac->ref.expression,
                                              ac->ref.result);
    pushCommand(new CalculatorHistoryCommand(ac->ref.expression));
    return;
  }

  if (auto openLink = std::dynamic_pointer_cast<Quicklink::Open>(action)) {
    openLink->open(completions());
  }

  clearSearch();
  qDebug() << "activated action:" << action->name();
}

void IndexCommand::onSearchChanged(const QString &text) {
  query = text;
  list->clear();

  std::string_view query(text.toLatin1().data());

  if (QColor(text).isValid()) {
    list->addSection("Color");

    auto circle = new ColorCircle(text, QSize(60, 60));

    circle->setStroke("#BBB", 3);

    auto colorLabel = new QLabel(text);

    colorLabel->setProperty("class", "transform-left");

    auto left = new VStack(colorLabel, new Chip("HEX"));
    auto right = new VStack(circle, new Chip(text));
    auto widget = new TransformResult(left, right);

    list->addWidgetItem(new CodeToColor(text), widget);
  }

  if (text.size() > 1) {
    Parser parser;

    if (auto result = parser.evaluate(query)) {
      auto value = result.value();
      list->addSection("Calculator");

      auto exprLabel = new QLabel(text);

      exprLabel->setProperty("class", "transform-left");

      auto answerLabel = new QLabel(QString::number(value.value));
      answerLabel->setProperty("class", "transform-left");

      auto left = new VStack(exprLabel, new Chip("Expression"));
      auto right = new VStack(
          answerLabel,
          new Chip(value.unit ? QString(value.unit->displayName.data())
                              : "Answer"));

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
