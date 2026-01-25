#pragma once
#include "clipboard-actions.hpp"
#include "common/entrypoint.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/action-pannel/action.hpp"

class DisableItemAction : public AbstractAction {
  EntrypointId m_id;

  void execute(ApplicationContext *ctx) override;

public:
  DisableItemAction(const EntrypointId &id);
};

class ResetItemRanking : public AbstractAction {
  EntrypointId m_id;
  void execute(ApplicationContext *context) override;

public:
  ResetItemRanking(const EntrypointId &id);
};

class MarkItemAsFavorite : public AbstractAction {
  QString m_id;

  void execute(ApplicationContext *ctx) override;

public:
  MarkItemAsFavorite(const QString &id);
};

class ToggleItemAsFavorite : public AbstractAction {
  EntrypointId m_id;
  bool m_value;

  void execute(ApplicationContext *ctx) override;
  QString title() const override;
  std::optional<ImageURL> icon() const override;

public:
  ToggleItemAsFavorite(const EntrypointId &id, bool currentValue);
};

class OpenItemPreferencesAction : public AbstractAction {
public:
  OpenItemPreferencesAction(const EntrypointId &id) : m_id(id) { setShortcut(Keybind::OpenSettings); }

  void execute(ApplicationContext *context) override;
  QString title() const override { return "Open Preferences"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("cog"); }

private:
  EntrypointId m_id;
};

class DisableApplication : public DisableItemAction {
  QString title() const override { return "Disable item"; }

public:
  DisableApplication(const EntrypointId &itemId) : DisableItemAction(itemId) {}
};

/**
 * Wrapper for the main action of a root item, automatically recording execution.
 */
class DefaultActionWrapper : public AbstractAction {
  std::unique_ptr<AbstractAction> m_action;
  EntrypointId m_id;

  void execute(ApplicationContext *context) override;

public:
  QString title() const override;

  DefaultActionWrapper(const EntrypointId &id, AbstractAction *action);
};

class SetRootItemAliasAction : public AbstractAction {
public:
  QString title() const override { return "Set alias"; }
  std::optional<ImageURL> icon() const override { return BuiltinIcon::Text; }
  void execute(ApplicationContext *context) override;
  SetRootItemAliasAction(EntrypointId id) : m_id(id) { setShortcut(Keybind::EditSecondaryAction); }

private:
  EntrypointId m_id;
};

// common actions applicable to all root search items
class RootSearchActionGenerator {
public:
  static std::vector<AbstractAction *> generateActions(const RootItem &item,
                                                       const RootItemMetadata &metadata) {
    const auto id = item.uniqueId();
    const auto copyId = new CopyToClipboardAction(Clipboard::Text(QString::fromStdString(id)), "Copy ID");
    const auto resetRanking = new ResetItemRanking(id);
    const auto markAsFavorite = new ToggleItemAsFavorite(id, metadata.favorite);
    const auto setAlias = new SetRootItemAliasAction(id);
    const auto openPreferences = new OpenItemPreferencesAction(id);
    const auto disable = new DisableApplication(id);

    disable->setShortcut(Keybind::RemoveAction);

    return {resetRanking, markAsFavorite, setAlias, openPreferences, copyId, disable};
  }
};
