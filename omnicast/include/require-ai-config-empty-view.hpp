#include "app.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "view.hpp"
#include "ui/empty-view.hpp"
#include <qboxlayout.h>

class RequireAiConfigEmptyView : public View {
  class ConfigureAction : public AbstractAction {
  public:
    void execute(AppWindow &app) override {
      app.popCurrentView();
      app.launchCommand("ai.configure-providers");
    }

    ConfigureAction() : AbstractAction("Configure providers", BuiltinOmniIconUrl("cog")) {}
  };

  EmptyViewWidget *emptyView = new EmptyViewWidget(this);

public:
  void onMount() override {
    hideInput();
    emptyView->setTitle("No AI provider configured");
    emptyView->setDescription("To make use of AI features, you need to enable at least one AI provider");
    emptyView->setIcon(BuiltinOmniIconUrl("stars").setBackgroundTint(ColorTint::Red));

    std::vector<ActionItem> items;
    auto action = std::make_unique<ConfigureAction>();

    action->setShortcut({.key = "return"});
    items.emplace_back(std::move(action));
    setActionPannel(std::move(items));
  }

  RequireAiConfigEmptyView(AppWindow &app) : View(app) {
    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(emptyView);
    setLayout(layout);
  }
};
