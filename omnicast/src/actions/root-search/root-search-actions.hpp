#pragma once
#include "ui/action-pannel/action.hpp"

class DisableItemAction : public AbstractAction {
  QString m_id;

  void execute() override;

public:
  DisableItemAction(const QString &id);
};

class ResetItemRanking : public AbstractAction {
  QString m_id;
  void execute() override;

public:
  ResetItemRanking(const QString &id);
};

class MarkItemAsFavorite : public AbstractAction {
  QString m_id;

  void execute() override;

public:
  MarkItemAsFavorite(const QString &id);
};

class DisableApplication : public DisableItemAction {
  QString title() const override { return "Disable Application"; }

public:
  DisableApplication(const QString &itemId) : DisableItemAction(itemId) {}
};

class DefaultActionWrapper : public AbstractAction {
  std::unique_ptr<AbstractAction> m_action;
  QString m_id;

  void execute() override;

public:
  QString title() const override;

  DefaultActionWrapper(const QString &id, AbstractAction *action);
};
