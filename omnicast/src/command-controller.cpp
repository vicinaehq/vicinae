#include "command-controller.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"

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

void CommandController::handleViewPoped(const BaseView *view) {
  if (m_frames.empty()) return;

  auto &frame = m_frames.back();

  frame->viewCount -= 1;

  if (frame->viewCount == 0) { m_frames.pop_back(); }
}

void CommandController::launch(const std::shared_ptr<AbstractCmd> &cmd) {
  // unload stalled no-view command
  if (!m_frames.empty() && m_frames.back()->viewCount == 0) { m_frames.pop_back(); }

  auto frame = std::make_unique<CommandFrame>();

  frame->context.reset(cmd->createContext(cmd));
  frame->command = cmd;
  frame->viewCount = 0;
  frame->context->setContext(m_ctx);
  m_frames.emplace_back(std::move(frame));
  m_frames.back()->context->load({});
}
