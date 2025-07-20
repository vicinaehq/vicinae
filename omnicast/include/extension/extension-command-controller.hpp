#pragma once
#include "extension/manager/extension-manager.hpp"

class ExtensionCommandController {
  ExtensionManager *m_manager = nullptr;
  QString m_sessionId;

public:
  void setSessionId(const QString &id) { m_sessionId = id; }
  void notify(const QString &handlerId, const QJsonArray &args) {
    QJsonObject payload;

    m_manager->emitGenericExtensionEvent(m_sessionId, handlerId, args);
  }

  ExtensionCommandController(ExtensionManager *manager) : m_manager(manager) {}
};
