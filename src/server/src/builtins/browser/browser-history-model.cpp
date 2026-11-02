#include "builtins/browser/browser-history-model.hpp"
#include "actions/app-actions.hpp"
#include "actions/clipboard-actions.hpp"
#include "actions/shortcut-actions.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"

QString BrowserHistorySection::displayId(const BrowserHistoryItem &item) const {
  return QString::fromStdString(item.uniqueId());
}

QString BrowserHistorySection::displayTitle(const BrowserHistoryItem &item) const {
  if (item.title.empty()) return item.host();
  return QString::fromStdString(item.title);
}

QString BrowserHistorySection::displaySubtitle(const BrowserHistoryItem &item) const { return item.host(); }

std::optional<ImageURL> BrowserHistorySection::displayIcon(const BrowserHistoryItem &item) const {
  return item.icon();
}

std::unique_ptr<ActionPanelState>
BrowserHistorySection::buildActionPanel(const BrowserHistoryItem &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();
  const QString url = QString::fromStdString(item.url);
  const QString title = QString::fromStdString(item.title);

  auto open = new OpenInBrowserAction(QUrl(url));
  open->setShortcut(Keyboard::Shortcut::enter());

  auto copyUrl = new CopyToClipboardAction(Clipboard::Text(url), tr("Copy URL"));
  copyUrl->setShortcut(Keybind::CopyAction);

  auto copyTitle = new CopyToClipboardAction(Clipboard::Text(title), tr("Copy Title"));

  auto convertToShortcut =
      new CreateShortcutAction({.link = url, .name = title}, tr("Convert to shortcut"),
                               ImageURL(BuiltinIcon::ArrowsContract).setBackgroundTint(SemanticColor::Red));

  auto toggleDetail = new StaticAction(m_detailVisible ? tr("Hide Details") : tr("Show Details"),
                                       BuiltinIcon::AppWindowSidebarRight, m_onToggleDetail);
  toggleDetail->setShortcut(Keyboard::Shortcut(Qt::Key_I, Qt::ControlModifier));

  auto remove =
      new StaticAction(tr("Delete from History"), BuiltinIcon::Trash, [item](ApplicationContext *ctx) {
        ctx->navigation->confirmAlert(
            tr("Delete from history?"),
            tr("Every visit to %1 will be removed from your browser history. This cannot be undone.")
                .arg(QString::fromStdString(item.url)),
            [ctx, item]() {
              const auto toast = ctx->services->toastService();

              ctx->services->browserExtension()->removeHistoryUrl(item).then(
                  toast, [toast](const std::expected<void, std::string> &result) {
                    if (!result) {
                      toast->failure(tr("Failed to delete from history: %1")
                                         .arg(QString::fromStdString(result.error())));
                      return;
                    }
                    toast->success(tr("Deleted from history"));
                  });
            });
      });
  remove->setShortcut(Keybind::DangerousRemoveAction);

  auto clear = new StaticAction(
      tr("Clear Browser History"), BuiltinIcon::Trash, [browserId = item.browserId](ApplicationContext *ctx) {
        const auto browser = ctx->services->browserExtension()->browser(browserId);

        if (!browser) {
          ctx->services->toastService()->failure(tr("Browser is no longer connected"));
          return;
        }

        const auto name = QString::fromStdString(browser->name);

        ctx->navigation->confirmAlert(
            tr("Clear all %1 history?").arg(name),
            tr("Your entire browsing history in %1 will be deleted. This cannot be undone.").arg(name),
            [ctx, browserId]() {
              const auto toast = ctx->services->toastService();

              ctx->services->browserExtension()->clearHistory(browserId).then(
                  toast, [toast](const std::expected<void, std::string> &result) {
                    if (!result) {
                      toast->failure(
                          tr("Failed to clear history: %1").arg(QString::fromStdString(result.error())));
                      return;
                    }
                    toast->success(tr("History cleared"));
                  });
            });
      });

  section->addAction(open);
  section->addAction(toggleDetail);
  section->addAction(copyUrl);
  section->addAction(copyTitle);
  section->addAction(convertToShortcut);

  auto dangerSection = panel->createSection();
  dangerSection->addAction(remove);
  dangerSection->addAction(clear);

  return panel;
}
