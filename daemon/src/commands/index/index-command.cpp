#include "commands/index/index-command.hpp"
#include "actions.hpp"
#include "app-database.hpp"
#include "calculator.hpp"
#include "command-database.hpp"
#include "command-object.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
#include "ui/color_circle.hpp"
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
#include <qmimedatabase.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qwidget.h>
#include <qwindowdefs.h>
#include <string_view>

class AppActionnable : public IActionnable {

public:
  std::shared_ptr<DesktopEntry> desktopFile;
  Service<AppDatabase> appDb;

  AppActionnable(Service<AppDatabase> appDb,
                 std::shared_ptr<DesktopEntry> desktopFile)
      : appDb(appDb), desktopFile(desktopFile) {}

  using Ref = const AppActionnable &;

  struct OpenAction : public IAction {
    std::shared_ptr<DesktopExecutable> action;

    QString name() const override { return action->name; }
    QIcon icon() const override { return action->icon(); }
    void exec(ExecutionContext ctx) override {
      action->launch();
      ctx.hideWindow();
      ctx.setSearch("");
    }

    OpenAction(std::shared_ptr<DesktopExecutable> action) : action(action) {}
  };

  ActionList generateActions() const override {
    ActionList list{
        std::make_shared<OpenAction>(this->desktopFile),
    };

    QMimeDatabase mimeDb;
    auto mime = mimeDb.mimeTypeForFile(desktopFile->path);

    for (const auto &action : desktopFile->actions) {
      list.push_back(std::make_shared<OpenAction>(action));
    }

    if (auto app = appDb.findBestOpenerForMime(mime)) {
      list.push_back(
          std::make_shared<OpenInAppAction>(app, "Open Desktop File"));
    }

    return list;
  }
};

class UseWithQuicklinkActionnable : public IActionnable {
  const Quicklink &link;

public:
  UseWithQuicklinkActionnable(const Quicklink &link) : link(link) {}

  ActionList generateActions() const override {
    return {std::make_shared<ActionnableQuicklink::OpenQuicklinkAction>(link)};
  }
};

IndexCommand::IndexCommand(AppWindow *app)
    : CommandObject(app), appDb(service<AppDatabase>()),
      quicklinkDb(service<QuicklistDatabase>()), list(new ManagedList()) {
  cmdDb = new CommandDatabase();

  for (const auto &cmd : cmdDb->commands) {
    if (!cmd.usableWith)
      continue;
    usableWithCommands.push_back(new Command(cmd));
  }

  auto layout = new QVBoxLayout();

  layout->setSpacing(0);
  layout->setAlignment(Qt::AlignTop);

  layout->addWidget(list, 1);

  layout->setContentsMargins(0, 10, 0, 0);

  connect(list, &ManagedList::itemSelected, this, &IndexCommand::itemSelected);
  connect(list, &ManagedList::itemActivated, this,
          &IndexCommand::itemActivated);

  forwardInputEvents(list);

  widget->setLayout(layout);

  onSearchChanged("");
}

void IndexCommand::onAttach() {
  setSearchPlaceholder("Search apps and commands...");
  setSearch(query);
  searchbar()->selectAll();
}

void IndexCommand::itemSelected(const IActionnable &item) {
  destroyCompletion();

  if (auto action = dynamic_cast<const ActionnableQuicklink *>(&item)) {
    auto &link = action->link;

    if (!link.placeholders.isEmpty()) {
      createCompletion(link.placeholders, link.iconName);
    }
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
  qDebug() << "activated action:" << action->name();
  action->exec(*this);
}

void IndexCommand::onSearchChanged(const QString &text) {
  query = text;
  list->clear();

  ExecutionContext ctx(*this);

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

    list->addWidgetItem(new CodeToColor(ctx, text), widget);
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

      list->addWidgetItem(
          new ActionnableCalculator(ctx, text, answerLabel->text()),
          new TransformResult(left, right));
    }
  }

  QList<std::shared_ptr<DesktopEntry>> apps;

  for (const auto app : appDb.apps) {
    if (!app->data.noDisplay && app->name.contains(text, Qt::CaseInsensitive))
      apps.push_back(app);
  }

  QList<Command> matchingCommands;

  for (const auto &cmd : cmdDb->commands) {
    if (!cmd.normalizedName.contains(text))
      continue;

    matchingCommands.push_back(cmd);
  }

  QList<std::shared_ptr<Quicklink>> links;
  QList<std::shared_ptr<Quicklink>> useWithQuicklinks;

  for (const auto &quicklink : quicklinkDb.list()) {
    if (text.size() > 0 &&
        quicklink->name.contains(text, Qt::CaseInsensitive)) {
      links.push_back(quicklink);
    }

    if (quicklink->placeholders.size() == 1)
      useWithQuicklinks.push_back(quicklink);
  }

  if (!apps.empty() || !matchingCommands.empty() || !links.empty()) {
    list->addSection("Results");
  }

  for (const auto &quicklink : links) {
    qDebug() << "quicklink matches " << quicklink->displayName;

    auto widget = new GenericListItem(QIcon::fromTheme(quicklink->iconName),
                                      quicklink->displayName, quicklink->url,
                                      "Quicklink");

    list->addWidgetItem(new ActionnableQuicklink(ctx, *quicklink), widget);
  }

  for (size_t i = 0; i != apps.size(); ++i) {
    auto &app = apps.at(i);
    auto widget =
        new GenericListItem(app->icon(), app->name, "", "Application");

    list->addWidgetItem(new AppActionnable(appDb, app), widget);
  }

  for (const auto &cmd : matchingCommands) {
    auto widget = new GenericListItem(QIcon::fromTheme(cmd.iconName), cmd.name,
                                      cmd.category, "Command");

    list->addWidgetItem(new ActionnableCommand(ctx, cmd), widget);
  }

  if (!useWithQuicklinks.isEmpty()) {
    list->addSection(QString("Use \"%1\" with...").arg(text));
  }

  for (const auto &link : useWithQuicklinks) {
    auto widget =
        new GenericListItem(QIcon::fromTheme(link->iconName), link->displayName,
                            link->url, "Quicklink");

    list->addWidgetItem(new UseWithQuicklinkActionnable(*link), widget);
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
