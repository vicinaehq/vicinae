#pragma once
#include "app-root-provider.hpp"
#include "app/app-database.hpp"
#include "argument.hpp"
#include "base-view.hpp"
#include "bookmark-actions.hpp"
#include "bookmark-service.hpp"
#include "clipboard-actions.hpp"
#include "command-actions.hpp"
#include "command-database.hpp"
#include "command-root-provider.hpp"
#include "root-bookmark-provider.hpp"
#include "root-item-manager.hpp"
#include "omni-command-db.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "app.hpp"
#include "command.hpp"
#include "omni-icon.hpp"
#include "quicklist-database.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/color-transform-widget.hpp"
#include "ui/declarative-omni-list-view.hpp"
#include "ui/omni-list-item-widget.hpp"
#include "ui/omni-list.hpp"
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"
#include <QtConcurrent/QtConcurrent>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <memory>
#include <qbrush.h>
#include <qcoreevent.h>
#include <qevent.h>
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

class RootSearchItem : public AbstractDefaultListItem, public ListView::Actionnable {
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

    void execute() override {
      auto manager = ServiceRegistry::instance()->rootItemManager();

      if (manager->registerVisit(m_id)) {
        qDebug() << "Visit registered";
      } else {
        qCritical() << "Failed to register visit";
      }

      m_action->execute();
    }

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
  const RootItem &item() const { return *m_item.get(); }
  RootSearchItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
  ~RootSearchItem() {}
};

class FallbackRootSearchItem : public AbstractDefaultListItem, public ListView::Actionnable {
  std::shared_ptr<RootItem> m_item;

  QList<AbstractAction *> generateActions() const override { return m_item->fallbackActions(); }

  QString id() const override { return QString("fallback.%1").arg(m_item->uniqueId()); }

  ItemData data() const override {
    return {
        .iconUrl = m_item->iconUrl(),
        .name = m_item->displayName(),
        .category = m_item->subtitle(),
        .accessories = m_item->accessories(),
    };
  }

public:
  FallbackRootSearchItem(const std::shared_ptr<RootItem> &item) : m_item(item) {}
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

class RootCommandV2 : public ListView {
  QTimer *m_calcDebounce = new QTimer(this);
  std::optional<CalculatorItem> m_currentCalculatorEntry;

  void renderEmpty() {
    m_list->beginResetModel();
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

    auto &section = m_list->addSection("Commands");

    for (auto item : commandItems)
      section.addItem(std::move(item));

    if (auto provider = rootManager->provider("apps")) {
      auto &section = m_list->addSection("Apps");
      auto items = provider->loadItems() | std::views::transform([](const auto &item) {
                     return std::make_unique<RootSearchItem>(item);
                   });

      for (auto item : items)
        section.addItem(std::move(item));
    }

    if (auto provider = rootManager->provider("bookmarks")) {
      auto &section = m_list->addSection("Bookmarks");
      auto items = provider->loadItems() | std::views::transform([](const auto &item) {
                     return std::make_unique<RootSearchItem>(item);
                   });

      for (auto item : items)
        section.addItem(std::move(item));
    }

    m_list->endResetModel(OmniList::SelectFirst);
  }

  void render(const QString &text) {
    auto rootItemManager = ServiceRegistry::instance()->rootItemManager();
    auto commandDb = ServiceRegistry::instance()->commandDb();
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();
    auto appDb = ServiceRegistry::instance()->appDb();
    auto calculator = ServiceRegistry::instance()->calculatorDb();
    const auto &quicklinks = quicklinkDb->list();
    const auto &appEntries = appDb->listEntries();
    const auto &commandEntries = commandDb->commands();
    size_t maxReserve = appEntries.size() + commandEntries.size() + quicklinks.size();

    qCritical() << "RENDER!";

    m_list->beginResetModel();

    auto start = std::chrono::high_resolution_clock::now();

    if (m_currentCalculatorEntry) {
      qDebug() << "calculator" << m_currentCalculatorEntry->expression << "="
               << m_currentCalculatorEntry->result;
      m_list->addSection("Calculator")
          .addItem(std::make_unique<BaseCalculatorListItem>(*m_currentCalculatorEntry));
    }

    if (QColor(text).isValid()) {
      m_list->addSection("Color").addItem(std::make_unique<ColorListItem>(text));
    }

    auto &results = m_list->addSection("Results");

    for (const auto &item : rootItemManager->prefixSearch(text.trimmed())) {
      results.addItem(std::make_unique<RootSearchItem>(item));
    }

    auto &fallbackSection = m_list->addSection(QString("Use \"%1\" with...").arg(text));

    auto fallbackItems =
        rootItemManager->allItems() | std::views::filter([rootItemManager](const auto &item) {
          return rootItemManager->isFallback(item->uniqueId());
        });

    for (const auto &fallback : fallbackItems) {
      fallbackSection.addItem(std::make_unique<FallbackRootSearchItem>(fallback));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    m_list->endResetModel(OmniList::SelectFirst);
    // qDebug() << "root searched in " << duration << "ms";
  }

  void onSearchChanged(const QString &text) override {
    QString query = text.trimmed();

    if (query.isEmpty()) return renderEmpty();

    m_calcDebounce->start();

    return render(text);
  }

  void handleCalculatorTimeout() {
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
    qCritical() << "calculator rerender";
    render(searchText());
  }

  void onActionExecuted(AbstractAction *action) override { qCritical() << "action title" << action->title(); }

  void initialize() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    connect(manager, &RootItemManager::itemsChanged, this, [this]() { onSearchChanged(searchText()); });
    connect(m_calcDebounce, &QTimer::timeout, this, &RootCommandV2::handleCalculatorTimeout);
    SimpleView::initialize();
  }

  void escapePressed() override {
    auto ui = ServiceRegistry::instance()->UI();

    if (searchText().isEmpty()) {
      ui->closeWindow();
      return;
    }

    clearSearchBar();
  }

public:
  RootCommandV2() {
    m_topBar->hideBackButton();
    setSearchPlaceholderText("Search for apps or commands...");
    m_calcDebounce->setInterval(100);
    m_calcDebounce->setSingleShot(true);
  }
};
