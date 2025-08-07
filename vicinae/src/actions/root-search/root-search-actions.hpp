#pragma once
#include "common.hpp"
#include "ui/action-pannel/action.hpp"

class DisableItemAction : public AbstractAction {
  QString m_id;

  void execute(ApplicationContext *ctx) override;

public:
  DisableItemAction(const QString &id);
};

class ResetItemRanking : public AbstractAction {
  QString m_id;
  void execute(ApplicationContext *context) override;

public:
  ResetItemRanking(const QString &id);
};

class MarkItemAsFavorite : public AbstractAction {
  QString m_id;

  void execute(ApplicationContext *ctx) override;

public:
  MarkItemAsFavorite(const QString &id);
};

class ToggleItemAsFavorite : public AbstractAction {
  QString m_id;
  bool m_value;

  void execute(ApplicationContext *ctx) override;
  QString title() const override;
  ImageURL icon() const override;

public:
  ToggleItemAsFavorite(const QString &id, bool currentValue);
};

class DisableApplication : public DisableItemAction {
  QString title() const override { return "Disable item"; }

public:
  DisableApplication(const QString &itemId) : DisableItemAction(itemId) {}
};

class UninstallExtensionAction : public AbstractAction {
  QString m_id;

  ImageURL icon() const override { return ImageURL::builtin("computer-chip"); }
  void execute(ApplicationContext *ctx) override;
  QString title() const override { return "Uninstall Extension"; }

public:
  UninstallExtensionAction(const QString &id) : m_id(id) { setStyle(AbstractAction::Danger); }
};

/**
 * Wrapper for the main action of a root item, automatically recording execution.
 */
class DefaultActionWrapper : public AbstractAction {
  std::unique_ptr<AbstractAction> m_action;
  QString m_id;

  void execute(ApplicationContext *context) override;

public:
  QString title() const override;

  DefaultActionWrapper(const QString &id, AbstractAction *action);
};
