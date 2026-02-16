#include "browser-extension.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/browser-tab-actions.hpp"
#include "builtin_icon.hpp"
#include "create-quicklink-command.hpp"
#include "navigation-controller.hpp"
#include "services/toast/toast-service.hpp"
#include "services/browser-extension-service.hpp"
#include "single-view-command-context.hpp"
#include "theme/colors.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/views/empty-view.hpp"
#include "ui/views/typed-list-view.hpp"

static bool guard(CommandController *ctrl) {
  const auto browserService = ctrl->context()->services->browserExtension();

  if (browserService->browsers().empty()) {
    auto empty = new EmptyView;

    empty->setData({.title = "No browser connected",
                    .description =
                        "You need to connect at least one browser to vicinae using the browser extension in "
                        "order to use this command.",
                    .icon = ImageURL(BuiltinIcon::Link).setFill(SemanticColor::Red)});

    ctrl->context()->navigation->pushView(empty);

    {
      auto panel = std::make_unique<ListActionPanelState>();
      auto main = panel->createSection();

      main->addAction(
          new OpenInBrowserAction(QUrl("https://docs.vicinae.com/browser-extension"), "Open documentation"));
      empty->setActions(std::move(panel));
    }

    return true;
  }

  return false;
}

class SearchBrowserTabsView : public FilteredTypedListView<BrowserExtensionService::BrowserTab> {
  FilteredItemData mapFilteredData(const BrowserExtensionService::BrowserTab &item) const override {
    AccessoryList accessories;

    if (item.audible) { accessories.emplace_back(item.muted ? "🔇" : "🔊"); }

    return {
        .id = item.uniqueId(),
        .title = item.title,
        .icon = item.icon(),
        .keywords = {item.url},
        .accessories = accessories,
    };
  }

  DataSet makeSet() {
    Section section;
    section.name = "Tabs ({count})";
    section.items = context()->services->browserExtension()->tabs();
    return {section};
  }

  DataSet initializeDataSet() override {
    const auto browser = context()->services->browserExtension();
    connect(browser, &BrowserExtensionService::tabsChanged, this, [this]() { setDataSet(makeSet()); });
    return makeSet();
  }

  QString initialSearchPlaceholderText() const override { return "Search, focus and close tabs"; }

  std::unique_ptr<ActionPanelState>
  createActionPanel(const BrowserExtensionService::BrowserTab &item) const override {
    return BrowserTabActionGenerator::generate(context(), item);
  }
};

class CreateShortcutFromActiveBrowserTabCommand : public GuardedBuiltinCallbackCommand {
  QString id() const override { return "shortcut-active-tab"; }
  QString name() const override { return "Create Shortcut from Active Tab"; }
  QString description() const override {
    return "Create a vicinae shortcut from the currently active browser tab. May yield unexpected results if "
           "many browsers are connected at once.";
  }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Link).setBackgroundTint(SemanticColor::Red);
  }

  void execute(CommandController *controller) const override {
    if (guard(controller)) return;

    const auto toast = controller->context()->services->toastService();
    const auto browser = controller->context()->services->browserExtension();
    const auto tab = browser->findActiveTab();

    if (!tab) {
      toast->failure("No active tab!");
      return;
    }

    auto view = new ShortcutFormView;

    view->setPrefilledValues(tab->url.c_str(), tab->title.c_str());
    controller->context()->navigation->pushView(view);
  }
};

class SearchBrowserTabsCommand : public GuardedBuiltinCallbackCommand {
  QString id() const override { return "browse-tabs"; }
  QString name() const override { return "Search Browser Tabs"; }
  QString description() const override { return "Search tabs from all connected browsers"; }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::AppWindowSidebarLeft).setBackgroundTint(SemanticColor::Red);
  }

  void execute(CommandController *controller) const override {
    if (guard(controller)) { return; }
    controller->context()->navigation->pushView(new SearchBrowserTabsView);
  }
};

BrowserExtension::BrowserExtension() {
  registerCommand<SearchBrowserTabsCommand>();
  registerCommand<CreateShortcutFromActiveBrowserTabCommand>();
}
