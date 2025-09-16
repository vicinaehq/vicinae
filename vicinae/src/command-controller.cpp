#include "command-controller.hpp"
#include "common.hpp"
#include "service-registry.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <qjsonobject.h>

CommandController::CommandController(ApplicationContext &ctx, const AbstractCmd &cmd,
                                     const LaunchProps &props)
    : m_ctx(ctx), m_cmd(cmd), m_props(props) {}

const LaunchProps &CommandController::launchProps() const { return m_props; }

ScopedLocalStorage CommandController::storage() const {
  return m_ctx.services->localStorage()->scoped(m_cmd.extensionId());
}

const AbstractCmd &CommandController::info() const { return m_cmd; }

QJsonObject CommandController::preferenceValues() const {
  QString id = QString("extension.%1").arg(m_cmd.uniqueId());

  return m_ctx.services->rootItemManager()->getPreferenceValues(id);
}

void CommandController::setPreferenceValues(const QJsonObject &value) const {
  QString id = QString("extension.%1").arg(m_cmd.uniqueId());

  m_ctx.services->rootItemManager()->setPreferenceValues(id, value);
}
