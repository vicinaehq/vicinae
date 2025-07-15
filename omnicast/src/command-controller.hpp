#include "base-view.hpp"
#include "command.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include <qobject.h>

class CommandController : public QObject {
public:
  struct CommandFrame {
    std::unique_ptr<CommandContext> context;
    std::shared_ptr<AbstractCmd> command;
    size_t viewCount;

    ~CommandFrame() { context->unload(); }
  };

  CommandController(NavigationController &controller);

  void launch(const std::shared_ptr<AbstractCmd> &cmd);

private:
  void handleViewPushed(const BaseView *view);
  void handleViewPoped(const BaseView *view);

  NavigationController &m_navigation;
  std::vector<std::unique_ptr<CommandFrame>> m_frames;
};
