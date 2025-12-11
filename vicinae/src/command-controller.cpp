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
  return m_ctx.services->rootItemManager()->getPreferenceValues(m_cmd.uniqueId());
}

void CommandController::setPreferenceValues(const QJsonObject &value) const {
  m_ctx.services->rootItemManager()->setPreferenceValues(m_cmd.uniqueId(), value);
}

const ApplicationContext *CommandController::context() const { return &m_ctx; }
