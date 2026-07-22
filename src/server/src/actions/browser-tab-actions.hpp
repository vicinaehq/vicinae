#pragma once
#include <QCoreApplication>
#include "builtin_icon.hpp"
#include "clipboard-actions.hpp"
#include "common/context.hpp"
#include "qml/shortcut-form-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/browser-extension-service.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-pannel/action.hpp"

struct BrowserTabActionGenerator {
  Q_DECLARE_TR_FUNCTIONS(BrowserTabActionGenerator)

public:
  static std::unique_ptr<ActionPanelState> generate(const ApplicationContext *ctx,
                                                    const BrowserExtensionService::BrowserTab &tab) {
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();

    auto focusTab =
        new StaticAction(tr("Switch to tab"), BuiltinIcon::Switch, [tab](ApplicationContext *ctx) {
          ctx->services->browserExtension()->focusTab(tab);
          ctx->navigation->closeWindow({.clearRootSearch = true});
        });

    focusTab->setShortcut(Keyboard::Shortcut::enter());

    auto convertToShortcut = new StaticAction(
        tr("Convert to shortcut"), BuiltinIcon::ArrowsContract, [tab](ApplicationContext *ctx) {
          auto view = new ShortcutFormViewHost;
          view->setPrefilledValues(tab.url.c_str(), tab.title.c_str());
          ctx->navigation->pushView(view);
          ctx->navigation->setNavigationTitle(tr("Convert tab to shortcut"));
          ctx->navigation->setNavigationIcon(
              ImageURL(BuiltinIcon::ArrowsContract).setBackgroundTint(SemanticColor::Red));
        });

    auto closeTab = new StaticAction(tr("Close tab"), BuiltinIcon::Trash, [tab](ApplicationContext *ctx) {
      if (const auto result = ctx->services->browserExtension()->closeTab(tab); !result) {
        ctx->services->toastService()->failure(tr("Failed to close tab: %1").arg(result.error().c_str()));
        return;
      }
      ctx->navigation->closeWindow({.clearRootSearch = true});
    });
    closeTab->setShortcut(Keybind::RemoveAction);

    auto copyUrl = new CopyToClipboardAction(Clipboard::Text(tab.url.c_str()), tr("Copy URL"));
    auto copyTitle = new CopyToClipboardAction(Clipboard::Text(tab.title.c_str()), tr("Copy Title"));
    auto copyId = new CopyToClipboardAction(Clipboard::Text(QString::number(tab.id)), tr("Copy ID"));

    copyUrl->setShortcut(Keybind::CopyAction);

    section->addAction(focusTab);
    section->addAction(convertToShortcut);
    section->addAction(closeTab);

    auto utilsSection = panel->createSection();

    utilsSection->addAction(copyUrl);
    utilsSection->addAction(copyTitle);
    utilsSection->addAction(copyId);

    return panel;
  }
};
