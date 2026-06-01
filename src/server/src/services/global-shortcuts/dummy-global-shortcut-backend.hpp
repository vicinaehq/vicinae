#pragma once
#include <limits>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

/**
 * Fallback used when no real backend is activatable. Accepts binds but never fires.
 */
class DummyGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_OBJECT

public:
  QString id() const override { return "dummy"; }
  bool isActivatable() const override { return true; }
  int activationPriority() const override { return std::numeric_limits<int>::min(); }

  bool start() override { return true; }
  void bindShortcut(const GlobalShortcutRequest &) override {}
  void unbindShortcut(const QString &) override {}
  std::optional<GlobalShortcutInfo> shortcut(const QString &) const override { return std::nullopt; }
};
