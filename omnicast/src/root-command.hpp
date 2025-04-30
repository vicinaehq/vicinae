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
#include <cfloat>
#include <chrono>
#include <cmath>
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
#include <qwidget.h>

static constexpr size_t RECENCY_WEIGHT = 2;
static constexpr size_t FREQUENCY_WEIGHT = 1;

// after more than 100 uses, the frequency is a differentiating factor no more
static constexpr size_t FREQUENCY_CAP = 100;

struct OpenBuiltinCommandAction : public AbstractAction {
  std::shared_ptr<AbstractCmd> cmd;
  QString text;

  void execute(AppWindow &app) override {
    LaunchProps props;
    props.arguments = app.topBar->m_completer->collect();

    qCritical() << "execute launch with" << props.arguments.size();

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

  double rankingScore() const override {
    using namespace std::chrono;

    double recencyScore = 0;

    if (m_entry.lastOpenedAt) {
      auto now = high_resolution_clock::now();
      double secondsSinceLastUse = duration_cast<seconds>(now - *m_entry.lastOpenedAt).count();

      recencyScore = std::exp(-0.1 * secondsSinceLastUse);
    }

    double frequencyScore = std::min(1.0, static_cast<double>(m_entry.openCount) / FREQUENCY_CAP);

    // qDebug() << "recency score" << m_entry.command->id() << recencyScore << "secs" << secondsSinceLastUse;

    return (recencyScore * RECENCY_WEIGHT) + (frequencyScore * FREQUENCY_WEIGHT);
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
      auto fileBrowser = appDb->fileBrowser();
      auto textEditor = appDb->textEditor();

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

    double rankingScore() const override {
      if (app->name().contains("Alacritty")) return 100;
      return 0;
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

    double rankingScore() const override { return DBL_MAX; }

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
  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> generateBaseSearch() {
    auto commandDb = ServiceRegistry::instance()->commandDb();
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();
    auto appDb = ServiceRegistry::instance()->appDb();

    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> list;
    list.push_back(std::make_unique<OmniList::VirtualSection>("Commands"));

    list.reserve(100);

    for (const auto &entry : commandDb->commands()) {
      if (entry.disabled) continue;

      list.push_back(std::make_unique<BuiltinCommandListItem>(entry));
    }

    for (const auto &link : quicklinkDb->list()) {
      list.push_back(std::make_unique<QuicklinkRootListItem>(link));
    }

    list.push_back(std::make_unique<OmniList::VirtualSection>("Apps"));

    for (auto &app : appDb->list()) {
      if (!app->displayable()) continue;

      list.push_back(std::make_unique<AppListItem>(app));
    }

    return list;
  }

  void onSearchChanged(const QString &s) override {
    DeclarativeOmniListView::onSearchChanged(s);
    m_calcDebounce->start();
  }

  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> generateList(const QString &s) override {
    auto commandDb = ServiceRegistry::instance()->commandDb();
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();
    auto appDb = ServiceRegistry::instance()->appDb();
    auto calculator = ServiceRegistry::instance()->calculatorDb();
    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> list;

    if (s.isEmpty()) return generateBaseSearch();

    auto start = std::chrono::high_resolution_clock::now();

    list.push_back(std::make_unique<OmniList::VirtualSection>("Calculator"));

    if (m_currentCalculatorEntry) {
      qDebug() << "calculator" << m_currentCalculatorEntry->expression << "="
               << m_currentCalculatorEntry->result;
      auto item = std::make_unique<BaseCalculatorListItem>(*m_currentCalculatorEntry);

      list.push_back(std::move(item));
    }

    if (QColor(s).isValid()) {
      list.push_back(std::make_unique<OmniList::VirtualSection>("Color"));
      list.push_back(std::make_unique<ColorListItem>(s));
    }

    list.push_back(std::make_unique<OmniList::VirtualSection>("Results"));

    if (!s.isEmpty()) {
      for (const auto &link : quicklinkDb->list()) {
        if (!link->name.startsWith(s, Qt::CaseInsensitive)) continue;

        auto quicklink = std::make_unique<QuicklinkRootListItem>(link);

        list.push_back(std::move(quicklink));
      }
    }

    if (!s.isEmpty()) {
      for (auto &app : appDb->list()) {
        if (!app->displayable()) continue;

        for (const auto &word : app->name().split(" ")) {
          if (!word.startsWith(s, Qt::CaseInsensitive)) continue;

          list.push_back(std::make_unique<AppListItem>(app));
          break;
        }
      }
    }

    for (auto &entry : commandDb->commands()) {
      if (entry.disabled || !entry.command->name().contains(s, Qt::CaseInsensitive)) continue;

      list.push_back(std::make_unique<BuiltinCommandListItem>(entry));
    }

    if (s.isEmpty()) {
      for (const auto &app : appDb->list()) {
        if (app->displayable()) { list.push_back(std::make_unique<AppListItem>(app)); }
      }
    }

    list.push_back(std::make_unique<OmniList::VirtualSection>(QString("Use \"%1\" with...").arg(s)));

    if (!s.isEmpty()) {
      for (const auto &link : quicklinkDb->list()) {
        if (link->placeholders.size() != 1) continue;

        list.push_back(std::make_unique<FallbackQuicklinkListItem>(link, s));
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // qDebug() << "root searched in " << duration << "ms";

    return list;
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

    list->setSorting({.enabled = true, .preserveSectionOrder = false});
  }
  ~RootView() {}
};
