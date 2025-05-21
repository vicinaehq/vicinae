#include "app.hpp"
#include "base-view.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/empty-view.hpp"
#include <qboxlayout.h>

class RequireAiConfigEmptyView : public FormView {
  class ConfigureAction : public AbstractAction {
  public:
    void execute(AppWindow &app) override {
      app.popCurrentView();
      app.launchCommand("ai.configure-providers");
    }

    ConfigureAction() : AbstractAction("Configure providers", BuiltinOmniIconUrl("cog")) {}
  };

  EmptyViewWidget *emptyView = new EmptyViewWidget(this);

  void configure() {
    auto ui = ServiceRegistry::instance()->UI();

    ui->popToRoot();
    // TODO: launch command
    // app.launchCommand("ai.configure-providers");
  }

public:
  void onActivate() override {
    emptyView->setTitle("No AI provider configured");
    emptyView->setDescription("To make use of AI features, you need to enable at least one AI provider");
    emptyView->setIcon(BuiltinOmniIconUrl("stars").setBackgroundTint(ColorTint::Red));

    std::vector<ActionItem> items;
    auto action =
        new StaticAction("Configure providers", BuiltinOmniIconUrl("cog"), [this]() { configure(); });

    action->setShortcut({.key = "return"});
    setActions({action});
  }

  RequireAiConfigEmptyView() {
    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(emptyView);
    setLayout(layout);
  }
};
