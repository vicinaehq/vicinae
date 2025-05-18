#pragma once
#include "argument.hpp"
#include "clipboard-actions.hpp"
#include "command-database.hpp"
#include "root-item-manager.hpp"
#include "omni-command-db.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
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
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"
#include <QtConcurrent/QtConcurrent>
#include <algorithm>
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
#include <quuid.h>
#include <qwidget.h>
#include <ranges>

/*
class CopyCalculatorResultAction : public CopyTextAction {
  CalculatorItem item;

public:
  void execute(AppWindow &app) override {
    auto calc = ServiceRegistry::instance()->calculatorDb();

    calc->insertComputation(item.expression, item.result);
    CopyTextAction::execute(app);
  }

  CopyCalculatorResultAction(const CalculatorItem &item, const QString &title, const QString &copyText)
      : CopyTextAction(title, copyText), item(item) {}
};
*/

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

class RootView : public DeclarativeOmniListView {
  AppWindow &app;

  class DisableItemAction : public AbstractAction {
    std::shared_ptr<RootItem> m_item;
    void execute(AppWindow &app) override {
      auto manager = ServiceRegistry::instance()->rootItemManager();

      if (manager->disableItem(m_item->uniqueId())) {
        app.statusBar->setToast("Item disabled", ToastPriority::Success);
      } else {
        app.statusBar->setToast("Failed to disabled item", ToastPriority::Danger);
      }
    }

  public:
    DisableItemAction(const std::shared_ptr<RootItem> &item)
        : AbstractAction("Disable item", BuiltinOmniIconUrl("trash")), m_item(item) {}
  };

  class DefaultActionWrapper : public AbstractAction {
    AbstractAction *m_action = nullptr;
    QString m_id;

    void execute(AppWindow &app) override {
      if (app.topBar->m_completer->isVisible()) {
        for (int i = 0; i != app.topBar->m_completer->m_args.size(); ++i) {
          auto &arg = app.topBar->m_completer->m_args.at(i);
          auto input = app.topBar->m_completer->m_inputs.at(i);

          qCritical() << "required" << arg.required << input->text();

          if (arg.required && input->text().isEmpty()) {
            input->setFocus();
            return;
          }
        }
      }

      auto manager = ServiceRegistry::instance()->rootItemManager();

      if (manager->registerVisit(m_id)) {
        qDebug() << "Visit registered";
      } else {
        qCritical() << "Failed to register visit";
      }

      m_action->execute(app);
    }

  public:
    QString title() const override { return m_action->title(); }

    DefaultActionWrapper(const QString &id, AbstractAction *action)
        : AbstractAction(action->title(), action->iconUrl), m_id(id), m_action(action) {}
    ~DefaultActionWrapper() { /*m_action->deleteLater();*/ }
  };

  class RootSearchItem : public AbstractDefaultListItem, public DeclarativeOmniListView::IActionnable {
    std::shared_ptr<RootItem> m_item;

    QList<AbstractAction *> generateActions() const override {
      auto baseActions = m_item->actions();
      QList<AbstractAction *> finalActions;

      finalActions.reserve(baseActions.size() + 2);

      for (int i = 0; i < baseActions.size(); ++i) {
        AbstractAction *action = baseActions.at(i);

        if (i == 0)
          finalActions.emplace_back(new DefaultActionWrapper(m_item->uniqueId(), action));
        else
          finalActions.emplace_back(action);
      }

      finalActions.emplace_back(new CopyToClipboardAction({}, "Reset ranking"));
      finalActions.emplace_back(new DisableItemAction(m_item));

      return finalActions;
    }

    ItemData data() const override {
      return {
          .iconUrl = m_item->iconUrl(),
          .name = m_item->displayName(),
          .category = m_item->subtitle(),
          .accessories = m_item->accessories(),
      };
    }

    std::unique_ptr<CompleterData> createCompleter() const override {
      return std::make_unique<CompleterData>(CompleterData{
          .iconUrl = m_item->iconUrl(),
          .arguments = m_item->arguments(),
      });
    }

    QString id() const override { return m_item->uniqueId(); }

  public:
    RootSearchItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
    ~RootSearchItem() {}
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
          /*
  new CopyCalculatorResultAction(item, "Copy result", sresult),
  new CopyCalculatorResultAction(item, "Copy expression",
                                 QString("%1 = %2").arg(item.expression).arg(sresult)),
          */

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
    auto rootManager = ServiceRegistry::instance()->rootItemManager();

    auto commandItems =
        rootManager->providers() | std::views::filter([](const auto &provider) {
          return provider->type() == RootProvider::Type::ExtensionProvider;
        }) |
        std::views::transform([](const auto &provider) { return provider->loadItems(); }) | std::views::join |
        std::views::transform([](const auto &item) { return std::make_unique<RootSearchItem>(item); });

    auto &section = list->addSection("Commands");

    for (auto item : commandItems)
      section.addItem(std::move(item));

    if (auto provider = rootManager->provider("apps")) {
      auto &section = list->addSection("Apps");
      auto items = provider->loadItems() | std::views::transform([](const auto &item) {
                     return std::make_unique<RootSearchItem>(item);
                   });

      for (auto item : items)
        section.addItem(std::move(item));
    }

    if (auto provider = rootManager->provider("bookmarks")) {
      auto &section = list->addSection("Bookmarks");
      auto items = provider->loadItems() | std::views::transform([](const auto &item) {
                     return std::make_unique<RootSearchItem>(item);
                   });

      for (auto item : items)
        section.addItem(std::move(item));
    }
  }

  void onSearchChanged(const QString &s) override {
    DeclarativeOmniListView::onSearchChanged(s);
    m_calcDebounce->start();
  }

  bool doesUseNewModel() const override { return true; }

  ItemList generateList(const QString &s) override { return {}; }

  void render(const QString &s) override {
    QString query = s.trimmed();

    if (query.isEmpty()) return renderBlankSearch();

    auto rootItemManager = ServiceRegistry::instance()->rootItemManager();
    auto commandDb = ServiceRegistry::instance()->commandDb();
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();
    auto appDb = ServiceRegistry::instance()->appDb();
    auto calculator = ServiceRegistry::instance()->calculatorDb();
    const auto &quicklinks = quicklinkDb->list();
    const auto &appEntries = appDb->listEntries();
    const auto &commandEntries = commandDb->commands();
    size_t maxReserve = appEntries.size() + commandEntries.size() + quicklinks.size();

    auto start = std::chrono::high_resolution_clock::now();

    if (m_currentCalculatorEntry) {
      qDebug() << "calculator" << m_currentCalculatorEntry->expression << "="
               << m_currentCalculatorEntry->result;
      list->addSection("Calculator")
          .addItem(std::make_unique<BaseCalculatorListItem>(*m_currentCalculatorEntry));
    }

    if (QColor(s).isValid()) { list->addSection("Color").addItem(std::make_unique<ColorListItem>(s)); }

    auto &results = list->addSection("Results");

    for (const auto &item : rootItemManager->prefixSearch(s.trimmed())) {
      results.addItem(std::make_unique<RootSearchItem>(item));
    }

    auto &fallbackCommands = list->addSection(QString("Use \"%1\" with...").arg(s));

    // TODO: use fallback commands

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // qDebug() << "root searched in " << duration << "ms";
  }

  void onMount() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();

    setSearchPlaceholderText("Search for apps or commands...");
    connect(manager, &RootItemManager::itemsChanged, this, [&]() { reload(OmniList::PreserveSelection); });
  }

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
