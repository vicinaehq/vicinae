#pragma once
#include "command.hpp"
#include "ui/action-pannel/action.hpp"

struct OpenBuiltinCommandAction : public AbstractAction {
  std::shared_ptr<AbstractCmd> cmd;
  QString text;
  bool m_forwardSearchText = false;

  void execute(ApplicationContext *context) override;

  // current search text will be forwarded to the new view
  void setForwardSearchText(bool value = true) { m_forwardSearchText = value; }

  OpenBuiltinCommandAction(const std::shared_ptr<AbstractCmd> &cmd, const QString &title = "Open command",
                           const QString &text = "")
      : AbstractAction(title, cmd->iconUrl()), cmd(cmd), text(text) {}
};
