#pragma once
#include "ui/views/base-view.hpp"
#include "command.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include <qobject.h>

class CommandController : public QObject {
public:
  struct CommandFrame {
    QObjectUniquePtr<CommandContext> context;
    std::shared_ptr<AbstractCmd> command;
    size_t viewCount;

    ~CommandFrame() {
      context->unload();
      qInfo() << "Unloading command" << command->uniqueId();
    }
  };

  CommandController(ApplicationContext *ctx);

  void launch(const std::shared_ptr<AbstractCmd> &cmd);
  void launch(const QString &id);

private:
  void handleViewPushed(const BaseView *view);
  void handleViewPoped(const BaseView *view);

  ApplicationContext *m_ctx;
  std::vector<std::unique_ptr<CommandFrame>> m_frames;
};
