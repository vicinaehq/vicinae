#include <QDesktopServices>
#include <QFile>
#include <QPointer>
#include "builtins/screenshots/screenshot-actions.hpp"
#include "actions/app-actions.hpp"
#include "actions/clipboard-actions.hpp"
#include "keyboard/keybind.hpp"
#include "services/screenshots/screenshot-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-panel/action-panel-state.hpp"

namespace {

void addRefresh(ActionPanelState &panel) {
  auto action = new StaticAction(QCoreApplication::translate("ScreenshotActions", "Refresh"),
                                 BuiltinIcon::ArrowClockwise,
                                 [](ApplicationContext *ctx) { ScreenshotActions::refresh(ctx); });
  action->setShortcut(Keybind::RefreshAction);
  panel.createSection()->addAction(action);
}

} // namespace

void ScreenshotActions::transfer(const Screenshot &item, bool paste, const ApplicationContext *ctx) {
  const bool recording = item.kind == Screenshot::Kind::Recording;
  auto toast = ctx->services->toastService();
  if (paste && !ctx->services->pasteService()->supportsPaste()) {
    toast->failure(tr("Cannot paste"), tr("Allow Accessibility access to paste into other apps."));
    return;
  }
  if (!QFile::exists(QString::fromStdString(item.path.string()))) {
    toast->failure(recording ? tr("Could not read recording") : tr("Could not read screenshot"),
                   tr("The file may have been moved or deleted."));
    ctx->services->screenshots()->refresh();
    return;
  }
  const Clipboard::File content{item.path};
  if (paste) {
    if (ctx->services->pasteService()->pasteContent(content)) {
      toast->clear();
      ctx->navigation->closeWindow();
    } else {
      toast->failure(recording ? tr("Could not paste recording") : tr("Could not paste screenshot"));
    }
  } else if (ctx->services->clipman()->copyContent(content)) {
    toast->clear();
    ctx->navigation->showHud(recording ? tr("Recording copied") : tr("Screenshot copied"),
                             BuiltinIcon::CopyClipboard);
  } else {
    toast->failure(recording ? tr("Could not copy recording") : tr("Could not copy screenshot"));
  }
}

void ScreenshotActions::pasteLast(const ApplicationContext *ctx) {
  auto service = ctx->services->screenshots();
  ctx->services->toastService()->dynamic(tr("Finding last screenshot..."));
  QObject::connect(
      service, &ScreenshotService::refreshed, ctx->navigation.get(),
      [ctx, service] {
        const auto item = service->latestImage();
        if (!item) {
          ctx->services->toastService()->failure(
              tr("No saved screenshots found"),
              service->error().value_or(tr("Take a screenshot and save it to a file first.")));
          return;
        }
        transfer(*item, true, ctx);
      },
      Qt::SingleShotConnection);
  service->refresh();
}

void ScreenshotActions::refresh(const ApplicationContext *ctx) {
  auto service = ctx->services->screenshots();
  auto toast = ctx->services->toastService();
  toast->setToast(tr("Refreshing..."), ToastStyle::Dynamic);
  const QPointer<Toast> pending(toast->currentToast());
  QObject::connect(
      service, &ScreenshotService::refreshed, ctx->navigation.get(),
      [service, toast, pending] {
        if (!pending || toast->currentToast() != pending) return;
        if (service->error()) {
          toast->failure(tr("Could not refresh all screenshots and recordings"), *service->error());
        } else {
          toast->success(tr("Refreshed"));
        }
      },
      Qt::SingleShotConnection);
  service->refresh();
}

std::unique_ptr<ActionPanelState> ScreenshotActions::panel(const Screenshot &item, ApplicationContext *ctx) {
  const auto path = item.path;
  const bool recording = item.kind == Screenshot::Kind::Recording;
  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();
  if (ctx->services->pasteService()->supportsPaste()) {
    auto paste = new StaticAction(recording ? tr("Paste Recording") : tr("Paste Screenshot"),
                                  BuiltinIcon::CopyClipboard,
                                  [item](ApplicationContext *ctx) { transfer(item, true, ctx); });
    paste->setPrimary(true);
    section->addAction(paste);
  }
  auto copy =
      new StaticAction(recording ? tr("Copy Recording") : tr("Copy Screenshot"), BuiltinIcon::CopyClipboard,
                       [item](ApplicationContext *ctx) { transfer(item, false, ctx); });
  copy->setShortcut(Keybind::CopyAction);
  section->addAction(copy);

  const auto filename = QString::fromStdString(path.string());
  section = panel->createSection();
  section->addAction(new StaticAction(recording ? tr("Open Recording") : tr("Open Screenshot"),
                                      recording ? BuiltinIcon::Video : BuiltinIcon::Image,
                                      [filename](ApplicationContext *ctx) {
                                        if (QDesktopServices::openUrl(QUrl::fromLocalFile(filename))) {
                                          ctx->navigation->closeWindow();
                                        } else {
                                          ctx->services->toastService()->failure(tr("Could not open file"));
                                        }
                                      }));
  section->addAction(new OpenWithAction(filename));
#ifdef Q_OS_MACOS
  const auto revealTitle = tr("Show in Finder");
#else
  const auto revealTitle = tr("Show in File Browser");
#endif
  section->addAction(new StaticAction(revealTitle, BuiltinIcon::Folder, [path](ApplicationContext *ctx) {
    if (ctx->services->appDb()->showInFileBrowser(path, true)) {
      ctx->navigation->closeWindow();
    } else {
      ctx->services->toastService()->failure(tr("Could not show file in file browser"));
    }
  }));
  section = panel->createSection();
  section->addAction(new CopyToClipboardAction(Clipboard::Text{filename}, tr("Copy File Path")));
  auto trash = new StaticAction(tr("Move to Trash"), BuiltinIcon::Trash, [filename](ApplicationContext *ctx) {
    if (QFile::moveToTrash(filename)) {
      ctx->services->toastService()->success(tr("File moved to Trash"));
      ctx->services->screenshots()->refresh();
    } else {
      ctx->services->toastService()->failure(tr("Could not move file to Trash"));
    }
  });
  trash->setStyle(AbstractAction::Style::Danger);
  panel->createSection()->addAction(trash);
  addRefresh(*panel);
  return panel;
}

std::unique_ptr<ActionPanelState> ScreenshotActions::emptyPanel() {
  auto panel = std::make_unique<ListActionPanelState>();
  addRefresh(*panel);
  return panel;
}
