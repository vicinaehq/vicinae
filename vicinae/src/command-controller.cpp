#include "command-controller.hpp"
#include "common.hpp"
#include "extension/extension-command.hpp"
#include "navigation-controller.hpp"
#include "extension/missing-extension-preference-view.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <qlogging.h>
#include <qobjectdefs.h>

CommandController::CommandController(ApplicationContext *ctx) : m_ctx(ctx) {
  connect(ctx->navigation.get(), &NavigationController::viewPushed, this,
          &CommandController::handleViewPushed);
  connect(ctx->navigation.get(), &NavigationController::viewPoped, this, &CommandController::handleViewPoped);
}

void CommandController::handleViewPushed(const BaseView *view) {
  if (m_frames.empty()) return;

  auto &frame = m_frames.back();

  frame->viewCount += 1;
}

const AbstractCmd *CommandController::activeCommand() const {
  if (m_frames.empty()) return nullptr;

  return m_frames.back()->command.get();
}

void CommandController::unloadActiveCommand() {
  if (m_frames.empty()) {
    qWarning() << "unloadActiveCommand called while no commands are loaded";
    return;
  }

  auto size = m_frames.back()->viewCount;

  for (int i = 0; i < size; ++i) {
    m_ctx->navigation->popCurrentView();
  }
}

bool CommandController::reloadActiveCommand() {
  auto cmd = activeCommand();

  if (!cmd) return false;

  // we only store the id, as the pointer will likely become invalid
  // after unloading.
  QString id = cmd->uniqueId();

  unloadActiveCommand();
  launch(id);

  return true;
}

void CommandController::handleViewPoped(const BaseView *view) {
  if (m_frames.empty()) return;

  auto &frame = m_frames.back();

  frame->viewCount -= 1;

  if (frame->viewCount == 0) { m_frames.pop_back(); }
}

void CommandController::launch(const QString &id) {
  if (auto cmd = m_ctx->services->commandDb()->findCommand(id)) { launch(cmd->command); };
}

void CommandController::launch(const std::shared_ptr<AbstractCmd> &cmd) {
  // unload stalled no-view command
  if (!m_frames.empty() && m_frames.back()->viewCount == 0) { m_frames.pop_back(); }

  auto itemId = QString("extension.%1").arg(cmd->uniqueId());
  auto manager = m_ctx->services->rootItemManager();
  auto preferences = manager->getMergedItemPreferences(itemId);
  auto preferenceValues = manager->getPreferenceValues(itemId);

  for (const auto &preference : preferences) {
    QJsonValue value = preferenceValues.value(preference.name());
    bool hasValue = !(value.isUndefined() || value.isNull());
    bool hasDefault = !preference.defaultValue().isUndefined();
    bool isMissing = preference.required() && !hasValue && !hasDefault;

    if (!isMissing) continue;

    if (cmd->type() == CommandType::CommandTypeExtension) {
      auto extensionCommand = std::static_pointer_cast<ExtensionCommand>(cmd);

      m_ctx->navigation->pushView(
          new MissingExtensionPreferenceView(extensionCommand, preferences, preferenceValues));
      m_ctx->navigation->setNavigationTitle(cmd->name());
      m_ctx->navigation->setNavigationIcon(cmd->iconUrl());
      return;
    }

    qDebug() << "MISSING PREFERENCE" << preference.title();
  }

  auto frame = std::make_unique<CommandFrame>();

  frame->context.reset(cmd->createContext(cmd));
  frame->command = cmd;
  frame->viewCount = 0;
  frame->context->setContext(m_ctx);
  m_frames.emplace_back(std::move(frame));
  m_frames.back()->context->load({});
}
