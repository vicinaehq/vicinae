#include "app.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action-list-item.hpp"
#include "ui/action-pannel/action-pannel-list-view.hpp"
#include "ui/action-pannel/action.hpp"
#include "wm/window-manager.hpp"
#include <qfuturewatcher.h>

class MoveWorkspaceAction : public AbstractAction {
  AbstractWindowManager &wm;

  void execute(AppWindow &app) override {
    // wm.focus();
  }

public:
  MoveWorkspaceAction(const AbstractWindowManager::Window &window, AbstractWindowManager &wm)
      : wm(wm), AbstractAction(window.title(), BuiltinOmniIconUrl("app-window")) {}
};

class MoveToWorkspaceView : public ActionPannelListView {
  AbstractWindowManager &wm;
  std::vector<std::unique_ptr<MoveWorkspaceAction>> _actions;

  std::vector<AbstractAction *> actions() const override {
    std::vector<AbstractAction *> actions;

    for (const auto &ac : _actions) {
      actions.push_back(ac.get());
    }

    return actions;
  }

  void finished() {}

public:
  MoveToWorkspaceView(AbstractWindowManager &wm) : wm(wm) {
    _list->beginUpdate();

    for (auto window : wm.listWindowsSync()) {
      auto waction = std::make_unique<MoveWorkspaceAction>(*window.get(), wm);

      _list->addItem(std::make_unique<ActionListItem>(waction.get()));
      _actions.push_back(std::move(waction));
    }

    _list->commitUpdate();
  }
};

class MoveToWorkspaceListAction : public AbstractAction {
  AbstractWindowManager &wm;

  void execute(AppWindow &app) override { app.actionPannel->pushView(new MoveToWorkspaceView(wm)); }
  bool isPushView() const override { return true; }

public:
  MoveToWorkspaceListAction(AbstractWindowManager &wm)
      : AbstractAction("Move to workspace", BuiltinOmniIconUrl("app-window")), wm(wm) {}
};
