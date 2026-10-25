#pragma once
#include <QCoreApplication>
#include "services/builtin-icon/builtin-icon.hpp"
#include "actions/clipboard-actions.hpp"
#include "builtins/browser/bookmark-tab-form-view-host.hpp"
#include "common/context.hpp"
#include "actions/shortcut-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/browser-extension-service.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"
#include "ui/action-panel/action.hpp"

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

    auto convertToShortcut = new CreateShortcutAction(
        {.link = tab.url.c_str(), .name = tab.title.c_str()}, tr("Convert to shortcut"),
        ImageURL(BuiltinIcon::ArrowsContract).setBackgroundTint(SemanticColor::Red));

    auto closeTab = new StaticAction(tr("Close tab"), BuiltinIcon::Trash, [tab](ApplicationContext *ctx) {
      if (const auto result = ctx->services->browserExtension()->closeTab(tab); !result) {
        ctx->services->toastService()->failure(tr("Failed to close tab: %1").arg(result.error().c_str()));
        return;
      }
      // Keep window open: root search refreshes via tabsChanged, built-in view reloads on the same signal.
    });
    closeTab->setShortcut(Keybind::RemoveAction);

    auto pinTab = new StaticAction(
        tab.pinned ? tr("Unpin tab") : tr("Pin tab"),
        tab.pinned ? BuiltinIcon::PinDisabled : BuiltinIcon::Pin, [tab](ApplicationContext *ctx) {
          auto result = ctx->services->browserExtension()->setPinned(tab.browserId, tab.id, !tab.pinned);
          if (!result)
            ctx->services->toastService()->failure(
                tr("Failed to update tab: %1").arg(result.error().c_str()));
        });

    auto muteTab = new StaticAction(
        tab.muted ? tr("Unmute tab") : tr("Mute tab"),
        tab.muted ? BuiltinIcon::SpeakerOn : BuiltinIcon::SpeakerOff, [tab](ApplicationContext *ctx) {
          auto result = ctx->services->browserExtension()->setMuted(tab.browserId, tab.id, !tab.muted);
          if (!result)
            ctx->services->toastService()->failure(
                tr("Failed to update tab: %1").arg(result.error().c_str()));
        });

    auto copyUrl = new CopyToClipboardAction(Clipboard::Text(tab.url.c_str()), tr("Copy URL"));
    auto copyTitle = new CopyToClipboardAction(Clipboard::Text(tab.title.c_str()), tr("Copy Title"));
    auto copyId = new CopyToClipboardAction(Clipboard::Text(QString::number(tab.id)), tr("Copy ID"));

    copyUrl->setShortcut(Keybind::CopyAction);

    section->addAction(focusTab);
    section->addAction(convertToShortcut);
    section->addAction(closeTab);
    section->addAction(pinTab);
    section->addAction(muteTab);
    section->addAction(
        new StaticAction(tr("Duplicate tab"), BuiltinIcon::Duplicate, [tab](ApplicationContext *ctx) {
          auto result = ctx->services->browserExtension()->duplicateTab(tab.browserId, tab.id);
          if (!result)
            ctx->services->toastService()->failure(
                tr("Failed to duplicate tab: %1").arg(result.error().c_str()));
        }));
    section->addAction(
        new StaticAction(tr("Bookmark tab"), BuiltinIcon::Bookmark, [tab](ApplicationContext *ctx) {
          ctx->navigation->pushView(new BookmarkTabFormViewHost(tab));
        }));

    auto utilsSection = panel->createSection();

    utilsSection->addAction(copyUrl);
    utilsSection->addAction(copyTitle);
    utilsSection->addAction(copyId);

    return panel;
  }
};
