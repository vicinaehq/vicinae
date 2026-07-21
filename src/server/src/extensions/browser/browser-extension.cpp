#include "browser-extension.hpp"
#include "actions/app/app-actions.hpp"
#include "builtin_icon.hpp"
#include "qml/browser-tabs-view-host.hpp"
#include "qml/empty-view-host.hpp"
#include "qml/shortcut-form-view-host.hpp"
#include "navigation-controller.hpp"
#include "services/toast/toast-service.hpp"
#include "services/browser-extension-service.hpp"
#include "single-view-command-context.hpp"
#include "theme/colors.hpp"
#include <QCoreApplication>

static bool guard(CommandController &ctrl) {
  const auto browserService = ctrl.context()->services->browserExtension();

  if (browserService->browsers().empty()) {
    auto empty = new EmptyViewHost(
        QCoreApplication::translate("browser-extension", "No browser connected"),
        QCoreApplication::translate("browser-extension",
                                    "You need to connect at least one browser to vicinae using the browser "
                                    "extension in order to use this command."),
        ImageURL(BuiltinIcon::Link).setFill(SemanticColor::Red));

    ctrl.context()->navigation->pushView(empty);

    auto panel = std::make_unique<ListActionPanelState>();
    auto main = panel->createSection();
    main->addAction(
        new OpenInBrowserAction(QUrl("https://docs.vicinae.com/browser-extension"),
                                QCoreApplication::translate("browser-extension", "Open documentation")));
    empty->setActions(std::move(panel));

    return true;
  }

  return false;
}

class CreateShortcutFromActiveBrowserTabCommand : public GuardedBuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(CreateShortcutFromActiveBrowserTabCommand)

  QString id() const override { return "shortcut-active-tab"; }
  QString name() const override { return tr("Create Shortcut from Active Tab"); }
  std::vector<QString> keywords() const override { return {"Create Shortcut from Active Tab"}; }
  QString description() const override {
    return tr("Create a vicinae shortcut from the currently active browser tab. May yield unexpected "
              "results if many browsers are connected at once.");
  }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Link).setBackgroundTint(SemanticColor::Red);
  }

  void execute(CommandController &controller) const override {
    if (guard(controller)) return;

    const auto toast = controller.context()->services->toastService();
    const auto browser = controller.context()->services->browserExtension();
    const auto tab = browser->findActiveTab();

    if (!tab) {
      toast->failure(tr("No active tab!"));
      return;
    }

    auto view = new ShortcutFormViewHost;

    view->setPrefilledValues(tab->url.c_str(), tab->title.c_str());
    controller.context()->navigation->pushView(view);
  }
};

class SearchBrowserTabsCommand : public GuardedBuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(SearchBrowserTabsCommand)

  QString id() const override { return "browse-tabs"; }
  QString name() const override { return tr("Search Browser Tabs"); }
  std::vector<QString> keywords() const override { return {"Search Browser Tabs"}; }
  QString description() const override { return tr("Search tabs from all connected browsers"); }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::AppWindowSidebarLeft).setBackgroundTint(SemanticColor::Red);
  }

  void execute(CommandController &controller) const override {
    if (guard(controller)) { return; }
    controller.context()->navigation->pushView(new BrowserTabsViewHost);
  }
};

BrowserExtension::BrowserExtension() {
  registerCommand<SearchBrowserTabsCommand>();
  registerCommand<CreateShortcutFromActiveBrowserTabCommand>();
}
