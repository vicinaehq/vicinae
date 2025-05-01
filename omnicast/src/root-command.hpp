#pragma once
#include "app/app-database.hpp"
#include "argument.hpp"
#include "command-database.hpp"
#include "edit-command-preferences-view.hpp"
#include "omni-command-db.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/action-pannel/open-with-action.hpp"
#include "app.hpp"
#include "command.hpp"
#include "omni-icon.hpp"
#include "quicklist-database.hpp"
#include "theme.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/color-transform-widget.hpp"
#include "ui/declarative-omni-list-view.hpp"
#include "ui/omni-list-item-widget.hpp"
#include "ui/omni-list.hpp"
#include "quicklink-actions.hpp"
#include "ui/top_bar.hpp"
#include <QtConcurrent/QtConcurrent>
#include <algorithm>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <iterator>
#include <memory>
#include <qbrush.h>
#include <qcoreevent.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qhash.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qmap.h>
#include <qmimedatabase.h>
#include <qnamespace.h>
#include <qnetworkcookiejar.h>
#include <qthreadpool.h>
#include <quuid.h>
#include <qwidget.h>
#include <ranges>

struct OpenBuiltinCommandAction : public AbstractAction {
  std::shared_ptr<AbstractCmd> cmd;
  QString text;

  void execute(AppWindow &app) override {
    LaunchProps props;
    props.arguments = app.topBar->m_completer->collect();

    app.launchCommand(cmd, {}, props);
  }

  OpenBuiltinCommandAction(const std::shared_ptr<AbstractCmd> &cmd, const QString &title = "Open command",
                           const QString &text = "")
      : AbstractAction(title, cmd->iconUrl()), cmd(cmd), text(text) {}
};

class OpenCommandPreferencesAction : public AbstractAction {
  std::shared_ptr<AbstractCmd> m_command;

  void execute(AppWindow &app) override {
    app.pushView(new EditCommandPreferencesView(app, m_command),
                 {.navigation = NavigationStatus{.title = QString("%1 - Preferences").arg(m_command->name()),
                                                 .iconUrl = m_command->iconUrl()}});
  }

public:
  OpenCommandPreferencesAction(const std::shared_ptr<AbstractCmd> &command)
      : AbstractAction("Edit preferences", BuiltinOmniIconUrl("pencil")), m_command(command) {}
};

class ColorListItem : public OmniList::AbstractVirtualItem, public DeclarativeOmniListView::IActionnable {
  QColor color;

  OmniListItemWidget *createWidget() const override {
    auto widget = new ColorTransformWidget();

    widget->setColor(color.name(), color);

    return widget;
  }

  QString id() const override { return color.name(); }

  int calculateHeight(int width) const override {
    static ColorTransformWidget *widget = nullptr;

    if (!widget) {
      widget = new ColorTransformWidget;
      widget->setColor("", "blue");
    }

    return widget->sizeHint().height();
  }

  QList<AbstractAction *> generateActions() const override { return {}; }

public:
  ColorListItem(QColor color) : color(color) {}
};

class DisableCommand : public AbstractAction {
  QString _id;
  bool _value;

  void execute(AppWindow &app) override {
    auto commandDb = ServiceRegistry::instance()->commandDb();

    commandDb->setDisable(_id, _value);
    app.statusBar->setToast("Command updated");
  }

public:
  DisableCommand(const QString &id, bool value)
      : AbstractAction(value ? "Disabled command" : "Enable command",
                       BuiltinOmniIconUrl(value ? "eye-disabled" : "eye")),
        _id(id), _value(value) {}
};

class BuiltinCommandListItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
  CommandDbEntry m_entry;

protected:
  QString text;

  QList<AbstractAction *> generateActions() const override {
    auto disable = new DisableCommand(m_entry.command->id(), true);
    QList<AbstractAction *> actions{new OpenBuiltinCommandAction(m_entry.command, "Open command", text),
                                    disable};

    if (!m_entry.command->preferences().empty()) {
      actions << new OpenCommandPreferencesAction(m_entry.command);
    }

    return actions;
  }

  std::unique_ptr<CompleterData> createCompleter() const override {
    if (m_entry.command->arguments().empty()) return nullptr;

    return std::make_unique<CompleterData>(
        CompleterData{.iconUrl = m_entry.command->iconUrl(), .arguments = m_entry.command->arguments()});
  }

  QString id() const override { return m_entry.command->id(); }

  ItemData data() const override {
    return {.iconUrl = m_entry.command->iconUrl(),
            .name = m_entry.command->name(),
            .category = m_entry.command->repositoryName(),
            .accessories = {{.text = "Command", .color = ColorTint::TextSecondary}}};
  }

public:
  BuiltinCommandListItem(const CommandDbEntry &entry, const QString &text = "")
      : m_entry(entry), text(text) {}
};

class QuicklinkRootListItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
public:
  std::shared_ptr<Quicklink> link;

  std::unique_ptr<CompleterData> createCompleter() const override {
    ArgumentList args;

    for (const auto &placeholder : link->placeholders) {
      CommandArgument arg;

      arg.type = CommandArgument::Text;
      arg.required = true;
      arg.placeholder = placeholder;
      arg.name = placeholder;
      args.emplace_back(arg);
    }

    return std::make_unique<CompleterData>(CompleterData{.iconUrl = iconUrl(), .arguments = args});
  }

  QList<AbstractAction *> generateActions() const override {
    auto open = new OpenCompletedQuicklinkAction(link);
    auto edit = new EditQuicklinkAction(link);
    auto duplicate = new DuplicateQuicklinkAction(link);
    auto remove = new RemoveQuicklinkAction(link);

    return {open, edit, duplicate, remove};
  }

  std::vector<ActionItem> generateActionPannel() const override {
    std::vector<ActionItem> items;

    items.push_back(ActionLabel("Quicklink"));
    items.push_back(std::make_unique<OpenCompletedQuicklinkAction>(link));
    items.push_back(std::make_unique<EditQuicklinkAction>(link));
    items.push_back(std::make_unique<DuplicateQuicklinkAction>(link));
    items.push_back(std::make_unique<OpenWithAction>(std::vector<QString>({})));
    items.push_back(std::make_unique<RemoveQuicklinkAction>(link));

    return items;
  }

  OmniIconUrl iconUrl() const {
    OmniIconUrl url(link->iconName);

    if (url.type() == OmniIconType::Builtin) { url.setBackgroundTint(ColorTint::Red); }

    return url;
  }

  ItemData data() const override {
    return {.iconUrl = iconUrl(),
            .name = link->name,
            .accessories = {{.text = "Quicklink", .color = ColorTint::TextSecondary}}};
  }

  QString id() const override { return QString("link-%1").arg(link->id); }

public:
  QuicklinkRootListItem(const std::shared_ptr<Quicklink> &link) : link(link) {}
  ~QuicklinkRootListItem() {}
};

class RootView : public DeclarativeOmniListView {
  AppWindow &app;

  class AppListItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
    std::shared_ptr<Application> app;

    QList<AbstractAction *> generateActions() const override {
      auto appDb = ServiceRegistry::instance()->appDb();
      QList<AbstractAction *> actions;
      auto fileBrowser = appDb->appProvider()->fileBrowser();
      auto textEditor = appDb->appProvider()->textEditor();

      actions << new OpenAppAction(app, "Open Application", {});

      for (const auto &desktopAction : app->actions()) {
        actions << new OpenAppAction(desktopAction, desktopAction->name(), {});
      }

      if (fileBrowser) { actions << new OpenAppAction(fileBrowser, "Open in folder", {app->id()}); }

      if (textEditor) { actions << new OpenAppAction(textEditor, "Open desktop file", {app->id()}); }

      actions << new CopyTextAction("Copy file path", app->id());

      return actions;
    }

    ItemData data() const override {
      return {.iconUrl = app->iconUrl(),
              .name = app->name(),
              .accessories = {{.text = "Application", .color = ColorTint::TextSecondary}}};
    }

    QString id() const override { return app->id(); }

  public:
    AppListItem(const std::shared_ptr<Application> &app) : app(app) {}
    ~AppListItem() {}
  };

  class FallbackQuicklinkListItem : public AbstractDefaultListItem, DeclarativeOmniListView::IActionnable {
    QString query;

  public:
    std::shared_ptr<Quicklink> link;

    QList<AbstractAction *> generateActions() const override {
      return {new OpenQuicklinkAction(link, {query}), new EditQuicklinkAction(link),
              new DuplicateQuicklinkAction(link)};
    }

    ItemData data() const override {
      return {.iconUrl = link->iconName,
              .name = link->name,
              .accessories = {{.text = "Quicklink", .color = ColorTint::TextSecondary}}};
    }

    QString id() const override { return QString("fallback-link-%1").arg(link->id); }

  public:
    FallbackQuicklinkListItem(const std::shared_ptr<Quicklink> &link, const QString &fallbackQuery)
        : link(link), query(fallbackQuery) {}
  };

  class BaseCalculatorListItem : public OmniList::AbstractVirtualItem,
                                 public DeclarativeOmniListView::IActionnable {
  protected:
    CalculatorItem item;

    OmniListItemWidget *createWidget() const override { return new CalculatorListItemWidget(item); }

    int calculateHeight(int width) const override {
      static CalculatorListItemWidget ruler({});

      return ruler.sizeHint().height();
    }

    QString id() const override { return item.expression; }

    QList<AbstractAction *> generateActions() const override {
      QString sresult = item.result;

      return {
          new CopyCalculatorResultAction(item, "Copy result", sresult),
          new CopyCalculatorResultAction(item, "Copy expression",
                                         QString("%1 = %2").arg(item.expression).arg(sresult)),

      };
    }

  public:
    BaseCalculatorListItem(const CalculatorItem &item) : item(item) {}
  };

public:
  QTimer *m_calcDebounce = new QTimer(this);
  std::optional<CalculatorItem> m_currentCalculatorEntry;

  // list without filtering
  void renderBlankSearch() {
    auto commandDb = ServiceRegistry::instance()->commandDb();
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();
    auto appDb = ServiceRegistry::instance()->appDb();
    const auto &quicklinks = quicklinkDb->list();
    const auto &appEntries = appDb->listEntries();
    const auto &commandEntries = commandDb->commands();
    size_t maxReserve = appEntries.size() + commandEntries.size() + quicklinks.size();

    auto filteredLinks =
        quicklinks | std::views::transform([](const auto &entry) {
          auto factory = [&entry]() { return std::make_unique<QuicklinkRootListItem>(entry); };
          return ShallowRankedListItem{.factory = factory, .rank = 0};
        });

    auto filteredApps = appEntries |
                        std::views::filter([](const auto &entry) { return entry.app->displayable(); }) |
                        std::views::transform([](const auto &entry) {
                          auto factory = [&entry]() { return std::make_unique<AppListItem>(entry.app); };

                          return ShallowRankedListItem{.factory = factory, .rank = entry.frecency};
                        });

    auto filteredCommands =
        commandEntries | std::views::filter([](const auto &entry) { return !entry.disabled; }) |
        std::views::transform([](const auto &entry) {
          auto factory = [&entry]() { return std::make_unique<BuiltinCommandListItem>(entry); };

          return ShallowRankedListItem{.factory = factory, .rank = 0};
        });

    {

      std::vector<ShallowRankedListItem> rankedResults;

      rankedResults.reserve(maxReserve);
      std::ranges::copy(filteredApps, std::back_inserter(rankedResults));
      std::ranges::copy(filteredCommands, std::back_inserter(rankedResults));
      std::ranges::copy(filteredLinks, std::back_inserter(rankedResults));
      std::ranges::sort(rankedResults, [](const auto &a, const auto &b) { return a.rank > b.rank; });

      auto &suggestions = list->addSection("Suggestions").withCapacity(5);
      auto topResults = rankedResults | std::views::take(5);

      std::ranges::for_each(topResults,
                            [&suggestions](const auto &ranked) { suggestions.addItem(ranked.factory()); });
    }

    {
      auto &commandSection = list->addSection("Commands");
      const auto &commands = commandDb->commands();
      const auto &links = quicklinkDb->list();

      commandSection.withCapacity(commands.size() + links.size());
      std::ranges::for_each(commands, [&commandSection](const auto entry) {
        commandSection.addItem(std::make_unique<BuiltinCommandListItem>(entry));
      });
      std::ranges::for_each(links, [&commandSection](const auto link) {
        commandSection.addItem(std::make_unique<QuicklinkRootListItem>(link));
      });
    }

    {
      const auto &appEntries = appDb->listEntries();
      auto &appSection = list->addSection("Apps").withCapacity(appEntries.size());
      auto filteredEntries = appEntries | std::views::filter([](const auto &entry) {
                               return !entry.disabled && entry.app->displayable();
                             });

      std::ranges::for_each(filteredEntries, [&appSection](const auto &entry) {
        appSection.addItem(std::make_unique<AppListItem>(entry.app));
      });
    }
  }

  void onSearchChanged(const QString &s) override {
    DeclarativeOmniListView::onSearchChanged(s);
    m_calcDebounce->start();
  }

  struct RankedListItem {
    std::unique_ptr<OmniList::AbstractVirtualItem> item;
    double rank;
  };

  struct ShallowRankedListItem {
    std::function<std::unique_ptr<OmniList::AbstractVirtualItem>(void)> factory;
    double rank;
  };

  bool doesUseNewModel() const override { return true; }

  ItemList generateList(const QString &s) override { return {}; }

  void render(const QString &s) override {
    if (s.isEmpty()) return renderBlankSearch();

    auto commandDb = ServiceRegistry::instance()->commandDb();
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();
    auto appDb = ServiceRegistry::instance()->appDb();
    auto calculator = ServiceRegistry::instance()->calculatorDb();
    const auto &quicklinks = quicklinkDb->list();
    const auto &appEntries = appDb->listEntries();
    const auto &commandEntries = commandDb->commands();
    size_t maxReserve = appEntries.size() + commandEntries.size() + quicklinks.size();

    auto isWordStart = [&s](const QString &text) -> bool {
      if (text.startsWith(s, Qt::CaseInsensitive)) { return true; }

      return std::ranges::any_of(
          text.split(" "), [&s](const QString &word) { return word.startsWith(s, Qt::CaseInsensitive); });
    };

    auto start = std::chrono::high_resolution_clock::now();

    if (m_currentCalculatorEntry) {
      qDebug() << "calculator" << m_currentCalculatorEntry->expression << "="
               << m_currentCalculatorEntry->result;
      list->addSection("Calculator")
          .addItem(std::make_unique<BaseCalculatorListItem>(*m_currentCalculatorEntry));
    }

    if (QColor(s).isValid()) { list->addSection("Color").addItem(std::make_unique<ColorListItem>(s)); }

    auto filteredLinks =
        quicklinks |
        std::views::filter([isWordStart](const auto &quicklink) { return isWordStart(quicklink->name); }) |
        std::views::transform([](const auto &entry) {
          auto item = std::make_unique<QuicklinkRootListItem>(entry);
          return RankedListItem{.item = std::move(item), .rank = 0};
        });

    auto filteredApps =
        appEntries | std::views::filter([](const auto &entry) { return entry.app->displayable(); }) |
        std::views::filter([&isWordStart](const auto &entry) { return isWordStart(entry.app->name()); }) |
        std::views::transform([](const auto &entry) {
          auto item = std::make_unique<AppListItem>(entry.app);

          return RankedListItem{.item = std::move(item), .rank = entry.frecency};
        });

    auto filteredCommands =
        commandEntries | std::views::filter([](const auto &entry) { return !entry.disabled; }) |
        std::views::filter([&isWordStart](const auto &entry) { return isWordStart(entry.command->name()); }) |
        std::views::transform([](const auto &entry) {
          auto item = std::make_unique<BuiltinCommandListItem>(entry);
          return RankedListItem{.item = std::move(item), .rank = 0};
        });

    std::vector<RankedListItem> rankedResults;

    {

      rankedResults.reserve(maxReserve);
      std::ranges::copy(filteredApps, std::back_inserter(rankedResults));
      std::ranges::copy(filteredCommands, std::back_inserter(rankedResults));
      std::ranges::copy(filteredLinks, std::back_inserter(rankedResults));

      auto sortRanked = [](const auto &a, const auto &b) { return a.rank > b.rank; };

      std::ranges::sort(rankedResults, sortRanked);

      auto &results = list->addSection("Results", QString::number(rankedResults.size()))
                          .withCapacity(rankedResults.size());

      std::ranges::for_each(rankedResults, [&results](auto &item) { results.addItem(std::move(item.item)); });
    }

    auto &fallbackCommands = list->addSection(QString("Use \"%1\" with...").arg(s));
    auto fallbackLinks = quicklinks | std::views::filter([&s](const auto &quicklink) {
                           return quicklink->placeholders.size() == 1;
                         });
    std::ranges::for_each(fallbackLinks, [&fallbackCommands, &s](const auto &link) {
      fallbackCommands.addItem(std::make_unique<FallbackQuicklinkListItem>(link, s));
    });

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // qDebug() << "root searched in " << duration << "ms";
  }

  void onMount() override {
    auto commandDb = ServiceRegistry::instance()->commandDb();

    setSearchPlaceholderText("Search for apps or commands...");
    connect(commandDb, &OmniCommandDatabase::commandRegistered, this, &RootView::handleRegisteredCommand);
  }

  void handleRegisteredCommand(const CommandDbEntry &entry) { reload(OmniList::SelectFirst); }

  RootView(AppWindow &app) : DeclarativeOmniListView(app), app(app) {
    m_calcDebounce->setInterval(100);
    m_calcDebounce->setSingleShot(true);

    connect(m_calcDebounce, &QTimer::timeout, this, [this]() {
      auto calculator = ServiceRegistry::instance()->calculatorDb();
      QString expression = searchText().trimmed();
      bool isComputable = false;

      for (const auto &ch : expression) {
        if (!ch.isLetterOrNumber() || ch.isSpace()) {
          isComputable = true;
          break;
        }
      }

      if (!isComputable) {
        m_currentCalculatorEntry.reset();
        return;
      }

      auto [result, ok] = calculator->quickCalculate(expression.toLatin1().data());

      if (ok) {
        m_currentCalculatorEntry = CalculatorItem{.expression = expression, .result = result.c_str()};
      } else {
        m_currentCalculatorEntry.reset();
      }
      reload(OmniList::SelectFirst);
    });

    // list->setSorting({.enabled = true, .preserveSectionOrder = false});
  }
  ~RootView() {}
};
