#include "browse-apps-view.hpp"
#include "actions/app/app-actions.hpp"
#include "clipboard-actions.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

class BrowseAppItem : public SearchableListView::Actionnable {
public:
  BrowseAppItem(const std::shared_ptr<AbstractApplication> &app) : m_app(app) {}

private:
  ItemData data() const override {
    bool active = !ServiceRegistry::instance()->windowManager()->findAppWindows(*m_app).empty();
    AccessoryList accessories;

    if (!m_app->displayable()) {
      accessories.emplace_back(ListAccessory{
          .text = "Hidden",
          .color = SemanticColor::Red,
          .fillBackground = true,
          .icon = ImageURL::builtin("eye-disabled"),
      });
    }

    return {
        .iconUrl = m_app->iconUrl(),
        .name = m_app->displayName(),
        .subtitle = m_app->description(),
        .accessories = accessories,
        .active = active,
    };
  }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto appDb = ctx->services->appDb();
    auto open = new OpenAppAction(m_app, "Open Application", {});
    auto copyId = new CopyToClipboardAction(Clipboard::Text(m_app->id()), "Copy App ID");
    auto copyLocation =
        new CopyToClipboardAction(Clipboard::Text(m_app->path().c_str()), "Copy App Location");

    auto mainSection = panel->createSection();
    auto utils = panel->createSection();
    auto itemSection = panel->createSection();
    auto appActions = m_app->actions();

    open->setClearSearch(true);
    panel->setTitle(m_app->displayName());

    auto activeWindows = ctx->services->windowManager()->findAppWindows(*m_app);

    if (!activeWindows.empty()) {
      auto focus = new FocusWindowAction(activeWindows.front());
      mainSection->addAction(focus);
      mainSection->addAction(open);
    } else {
      mainSection->addAction(open);
    }

    auto makeAction = [](auto &&pair) -> OpenAppAction * {
      const auto &[index, appAction] = pair;
      auto openAction = new OpenAppAction(appAction, appAction->name(), {});

      if (index < 9) {
        openAction->setShortcut({.key = QString::number(index + 1), .modifiers = {"ctrl", "shift"}});
      }

      return openAction;
    };

    auto actions = m_app->actions();

    for (int i = 0; i != appActions.size(); ++i) {
      auto &action = actions[i];
      auto openAction = new OpenAppAction(action, action->displayName(), {});

      if (i < 9) { openAction->setShortcut(QString("control+shift+%1").arg(i + 1)); }
      mainSection->addAction(openAction);
    }

    if (auto opener = appDb->findDefaultOpener(m_app->path().c_str())) {
      auto openLocation = new OpenAppAction(opener, "Open Location", {m_app->path().c_str()});
      openLocation->setShortcut(Keybind::OpenAction);
      utils->addAction(openLocation);
    }

    utils->addAction(copyId);
    utils->addAction(copyLocation);

    return panel;
  }

  std::vector<QString> searchStrings() const override {
    std::vector<QString> strs{m_app->displayName()};

    for (const auto &kw : m_app->keywords()) {
      strs.emplace_back(kw);
    }

    return strs;
  }

  std::shared_ptr<AbstractApplication> m_app;
};

BrowseAppsView::Data BrowseAppsView::initData() const {
  auto preferences = command()->preferenceValues();
  auto config = context()->services->config();
  auto appDb = context()->services->appDb();
  Data data;
  AppListOptions opts{.sortAlphabetically = preferences.value("sortAlphabetically").toBool()};
  bool showHidden = preferences.value("showHidden").toBool();

  for (const auto &app : appDb->list(opts)) {
    if (!showHidden && !app->displayable()) continue;
    data.emplace_back(std::make_shared<BrowseAppItem>(app));
  }

  return data;
}
