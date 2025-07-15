#include "command-controller.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"

CommandController::CommandController(NavigationController &controller) : m_navigation(controller) {
  connect(&m_navigation, &NavigationController::viewPushed, this, &CommandController::handleViewPushed);
  connect(&m_navigation, &NavigationController::viewPoped, this, &CommandController::handleViewPoped);
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
  auto frame = std::make_unique<CommandFrame>();

  frame->context.reset(cmd->createContext(cmd));
  frame->command = cmd;
  frame->viewCount = 0;
  frame->context->load({});
  m_frames.emplace_back(std::move(frame));
}
