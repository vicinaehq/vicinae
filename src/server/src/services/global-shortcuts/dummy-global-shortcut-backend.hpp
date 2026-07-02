#pragma once
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"
#include <expected>

/**
 * Fallback used when no real backend is activatable. Accepts binds but never fires.
 */
class DummyGlobalShortcutBackend : public AbstractGlobalShortcutBackend {
  Q_OBJECT

public:
  QString id() const override { return "dummy"; }
  bool isSupported() const override { return false; }

  bool start() override { return true; }
  std::expected<void, QString> bindShortcut(const GlobalShortcutRequest &) override {
    return std::unexpected(QStringLiteral("global shortcuts are not supported in this environment"));
  }
  void unbindShortcut(const QString &) override {}
  void unbindAll() override {}
};
