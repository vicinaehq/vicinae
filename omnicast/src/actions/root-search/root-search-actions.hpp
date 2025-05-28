#pragma once
#include "root-item-manager.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"

class DisableItemAction : public AbstractAction {
  QString m_id;

  void execute() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    auto ui = ServiceRegistry::instance()->UI();

    if (manager->disableItem(m_id)) {
      ui->setToast("Item disabled", ToastPriority::Success);
    } else {
      ui->setToast("Failed to disable", ToastPriority::Danger);
    }
  }

public:
  DisableItemAction(const QString &id)
      : AbstractAction("Disable item", BuiltinOmniIconUrl("trash")), m_id(id) {
    setStyle(AbstractAction::Danger);
  }
};

class ResetItemRanking : public AbstractAction {
  QString m_id;

  void execute() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    auto ui = ServiceRegistry::instance()->UI();

    // TODO: reset ranking
  }

public:
  ResetItemRanking(const QString &id)
      : AbstractAction("Reset ranking", BuiltinOmniIconUrl("arrow-counter-clockwise")), m_id(id) {}
};

class MarkItemAsFavorite : public AbstractAction {
  QString m_id;

  void execute() override {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    auto ui = ServiceRegistry::instance()->UI();

    // TODO: mark as favorite
  }

public:
  MarkItemAsFavorite(const QString &id)
      : AbstractAction("Mark as favorite", BuiltinOmniIconUrl("stars")), m_id(id) {}
};

class DisableApplication : public DisableItemAction {
  QString title() const override { return "Disable Application"; }

public:
  DisableApplication(const QString &itemId) : DisableItemAction(itemId) {}
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

public:
  QString title() const override { return m_action->title(); }

  DefaultActionWrapper(const QString &id, AbstractAction *action)
      : AbstractAction(action->title(), action->iconUrl), m_id(id), m_action(action) {
    setPrimary(true);
    setShortcut({.key = "return"});
  }
  ~DefaultActionWrapper() { m_action->deleteLater(); }
};
