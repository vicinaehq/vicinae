#pragma once
#include "app.hpp"
#include "calculator-database.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/omni-list-view.hpp"
#include "ui/omni-list.hpp"
#include <memory>
#include <qnamespace.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qthreadpool.h>
#include <qtmetamacros.h>

class RemoveCalculatorHistoryEntryAction : public AbstractAction {
  uint id;

public:
  RemoveCalculatorHistoryEntryAction(uint id)
      : AbstractAction("Remove entry", BuiltinOmniIconUrl("trash")), id(id) {}

  void execute(AppWindow &app) override {
    auto calc = ServiceRegistry::instance()->calculatorDb();
    bool removed = calc->removeById(id);

    if (removed) {
      app.statusBar->setToast("Entry removed");
    } else {
      app.statusBar->setToast("Failed to remove entry");
    }
  }
};

class CalculatorHistoryListItem : public AbstractDefaultListItem, public OmniListView::IActionnable {
  using RemoveCallback = std::function<void(const QString &id)>;

  RemoveCallback _rmCb;
  CalculatorEntry _entry;

public:
  QString id() const override { return QString("history-entry-%1").arg(_entry.id); };

  const CalculatorEntry &entry() const { return _entry; }

  QList<AbstractAction *> generateActions() const override {
    auto removeAction = new RemoveCalculatorHistoryEntryAction(_entry.id);

    QObject::connect(removeAction, &RemoveCalculatorHistoryEntryAction::didExecute, [this]() {
      if (_rmCb) _rmCb(id());
    });

    return {
        // new CopyTextAction("Copy result", _entry.result),
        // new CopyTextAction("Copy expression", _entry.expression),
        removeAction,
    };
  }

  void setRemoveCallback(const RemoveCallback &cb) { _rmCb = cb; }

  ItemData data() const override {
    return {
        .iconUrl = BuiltinOmniIconUrl("calculator"), .name = _entry.expression, .category = _entry.result};
  }

  CalculatorHistoryListItem(const CalculatorEntry &entry) : _entry(entry) {}
};

class CalculatorHistoryView : public OmniListView {
  void handleRemove(const QString &id) { list->removeItem(id); }

  void buildSearch(ItemList &items, const QString &s) override {
    auto calc = ServiceRegistry::instance()->calculatorDb();

    items.push_back(std::make_unique<OmniList::VirtualSection>("History"));
    for (const auto &history : calc->listAll()) {
      if (!history.expression.contains(s, Qt::CaseInsensitive)) continue;

      auto item = std::make_unique<CalculatorHistoryListItem>(history);

      item->setRemoveCallback(std::bind_front(&CalculatorHistoryView::handleRemove, this));
      items.push_back(std::move(item));
    }
  }

  void onMount() override {
    setSearchPlaceholderText("Do maths, convert units or search past calculations...");
  }

public:
  CalculatorHistoryView(AppWindow &app) : OmniListView(app) {}
};
