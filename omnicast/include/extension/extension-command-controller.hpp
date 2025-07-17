#pragma once
#include "extension/manager/extension-manager.hpp"

class ExtensionCommandController {
  ExtensionManager *m_manager = nullptr;
  QString m_sessionId;

public:
  void setSessionId(const QString &id) { m_sessionId = id; }
  void notify(const QString &handlerId, const QJsonArray &args) {
    QJsonObject payload;

    payload["args"] = args;
    m_manager->emitExtensionEvent(m_sessionId, handlerId, payload);
  }

  ExtensionCommandController(ExtensionManager *manager) : m_manager(manager) {}
};
