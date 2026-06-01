#pragma once
#include <optional>
#include <unordered_map>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QTimer>
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

class QDBusInterface;

/**
 * Global shortcuts via the XDG org.freedesktop.portal.GlobalShortcuts portal.
 * The compositor owns the binding: we register actions by id (+ description) and observe the
 * resolved trigger as a human-readable `triggerDisplay`. Selected over the input server where the
 * portal is available, since it suppresses the keystroke properly.
 */
class GlobalShortcutPortalBackend : public AbstractGlobalShortcutBackend {
  Q_OBJECT

public:
  GlobalShortcutPortalBackend();

  QString id() const override { return "xdg-portal"; }
  bool isActivatable() const override;
  int activationPriority() const override { return 200; }

  bool start() override;
  void bindShortcut(const GlobalShortcutRequest &request) override;
  void unbindShortcut(const QString &id) override;
  std::optional<GlobalShortcutInfo> shortcut(const QString &id) const override;

private slots:
  void onCreateSessionResponse(uint response, const QVariantMap &results);
  void onBindResponse(uint response, const QVariantMap &results);
  void onActivated(const QDBusObjectPath &sessionHandle, const QString &shortcutId, qulonglong timestamp,
                   const QVariantMap &options);

private:
  QString newToken(const QString &prefix) const;
  void createSession();
  void bindShortcuts();
  void closeSession();

  QDBusConnection m_bus;
  QDBusInterface *m_iface = nullptr;
  QString m_sessionHandle;
  bool m_sessionReady = false;
  bool m_started = false;
  QTimer m_applyTimer;
  std::unordered_map<QString, GlobalShortcutRequest> m_desired;
  std::unordered_map<QString, GlobalShortcutInfo> m_info;
};
