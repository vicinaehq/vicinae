#pragma once
#include "app.hpp"
#include "base-view.hpp"
#include "calculator-database.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
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

class CalculatorHistoryListItem : public AbstractDefaultListItem, public ListView::Actionnable {
  using RemoveCallback = std::function<void(const QString &id)>;

  RemoveCallback _rmCb;
  CalculatorEntry _entry;

public:
  QString generateId() const override { return QString("history-entry-%1").arg(_entry.id); };

  const CalculatorEntry &entry() const { return _entry; }

  QList<AbstractAction *> generateActions() const override {
    auto removeAction = new RemoveCalculatorHistoryEntryAction(_entry.id);

    QObject::connect(removeAction, &RemoveCalculatorHistoryEntryAction::didExecute, [this]() {
      if (_rmCb) _rmCb(generateId());
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

class CalculatorHistoryView : public ListView {
  void onSearchChanged(const QString &text) override {
    auto calc = ServiceRegistry::instance()->calculatorDb();

    m_list->beginResetModel();
    auto &section = m_list->addSection("History");

    for (const auto &history : calc->listAll()) {
      if (!history.expression.contains(text, Qt::CaseInsensitive)) continue;

      auto item = std::make_unique<CalculatorHistoryListItem>(history);

      section.addItem(std::move(item));
    }

    m_list->endResetModel(OmniList::SelectFirst);
  }

public:
  CalculatorHistoryView() {
    setSearchPlaceholderText("Do maths, convert units or search past calculations...");
  }
};
