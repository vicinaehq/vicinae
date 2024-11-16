#include "index-command.hpp"
#include "command-database.hpp"
#include "tinyexpr.hpp"
#include "xdg-desktop-database.hpp"
#include <cmath>
#include <memory>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <string_view>

static QString opset = "+-/*^%";

static bool isMathExpr(const QString &s) {
  for (auto c : s) {
    if (opset.contains(c))
      return true;
  }

  return false;
}

IndexCommand::IndexCommand(QWidget *parent)
    : QWidget(parent), input(new QLineEdit()), list(new QListWidget()),
      statusBar(new StatusBar()) {

  xdg = std::make_unique<XdgDesktopDatabase>(XdgDesktopDatabase());
  cmdDb = std::make_unique<CommandDatabase>(CommandDatabase());

  for (const auto &cmd : cmdDb->commands) {
    if (!cmd.usableWith)
      continue;
    usableWithCommands.push_back(cmd);
  }

  auto layout = new QVBoxLayout();

  input->setPlaceholderText("Search for apps or commands...");
  input->setTextMargins(15, 20, 15, 20);

  list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list->setFocusPolicy(Qt::NoFocus);
  list->setSpacing(0);

  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setAlignment(Qt::AlignTop);

  layout->addWidget(input);
  layout->addWidget(list, 1);
  layout->addWidget(statusBar);

  connect(input, &QLineEdit::textChanged, this,
          &IndexCommand::inputTextChanged);
  connect(list, &QListWidget::currentRowChanged, this,
          [](int n) { std::cout << "row changed: " << n << std::endl; });

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

void IndexCommand::inputTextChanged(const QString &text) {
  list->clear();

  std::string_view query(text.toLatin1().data());
  size_t labelIdx = 0;

  if (QColor(text).isValid()) {
    auto item = new QListWidgetItem(list);
    auto circle = new ColorCircle(text, QSize(60, 60));

    auto colorLabel = new QLabel(text);

    colorLabel->setProperty("class", "transform-left");

    auto left = new VStack(colorLabel, new Chip("HEX"));
    auto right = new VStack(circle, new Chip(text));
    auto widget = new TransformResult(left, right);

    list->addItem(item);
    list->setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());

    ++labelIdx;
  }

  if (text.size() > 1) {
    te_parser parser;

    if (double result = parser.evaluate(query); !std::isnan(result)) {
      // label
      auto item = new QListWidgetItem(list);
      auto widget = new QLabel("Calculator");

      widget->setContentsMargins(8, labelIdx > 0 ? 10 : 0, 0, 10);
      item->setFlags(item->flags() & !Qt::ItemIsSelectable);
      widget->setProperty("class", "minor category-name");

      list->addItem(item);
      list->setItemWidget(item, widget);
      item->setSizeHint(widget->sizeHint());
      ++labelIdx;
      // end label

      item = new QListWidgetItem(list);
      auto exprLabel = new QLabel(text);

      exprLabel->setProperty("class", "transform-left");

      auto answerLabel = new QLabel(QString::number(result));
      answerLabel->setProperty("class", "transform-left");

      auto left = new VStack(exprLabel, new Chip("Expression"));
      auto right = new VStack(answerLabel, new Chip("Answer"));
      auto w = new TransformResult(left, right);

      list->addItem(item);
      list->setItemWidget(item, w);
      item->setSizeHint(w->sizeHint());

      ++labelIdx;
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
    auto item = new QListWidgetItem(list);
    auto widget = new QLabel("Results");

    widget->setContentsMargins(8, labelIdx > 0 ? 10 : 0, 0, 10);
    item->setFlags(item->flags() & !Qt::ItemIsSelectable);
    widget->setProperty("class", "minor category-name");

    list->addItem(item);
    list->setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());
    ++labelIdx;
  }

  for (size_t i = 0; i != apps.size(); ++i) {
    auto &app = apps.at(i);

    auto item = new QListWidgetItem(list);
    auto widget = new GenericListItem(QString::fromStdString(app.icon),
                                      QString::fromStdString(app.name),
                                      "Extension", "Application");

    list->addItem(item);
    list->setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());
  }

  for (const auto &cmd : matchingCommands) {
    auto item = new QListWidgetItem(list);
    auto widget =
        new GenericListItem(cmd.iconName, cmd.name, cmd.category, "Command");

    list->addItem(item);
    list->setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());
  }

  if (usableWithCommands.size() > 0) {
    auto item = new QListWidgetItem(list);
    auto widget = new QLabel(QString("Use \"%1\" with...").arg(text));

    widget->setContentsMargins(8, labelIdx > 0 ? 10 : 0, 0, 10);
    item->setFlags(item->flags() & !Qt::ItemIsSelectable);
    widget->setProperty("class", "minor category-name");

    list->addItem(item);
    list->setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());
  }

  for (const auto &cmd : usableWithCommands) {
    auto item = new QListWidgetItem(list);
    auto widget =
        new GenericListItem(cmd.iconName, cmd.name, cmd.category, "Command");

    list->addItem(item);
    list->setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());
  }

  for (int i = 0; i != list->count(); ++i) {
    auto item = list->item(i);

    if (!item->flags().testFlag(Qt::ItemIsSelectable))
      continue;

    list->setCurrentItem(item);
    break;
  }
}
