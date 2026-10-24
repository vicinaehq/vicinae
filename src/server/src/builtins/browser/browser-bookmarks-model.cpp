#include "builtins/browser/browser-bookmarks-model.hpp"
#include "actions/app-actions.hpp"
#include "actions/clipboard-actions.hpp"
#include "actions/shortcut-actions.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "theme/colors.hpp"

QString BrowserBookmarksSection::displayId(const BrowserBookmark &bookmark) const {
  return QString::fromStdString(bookmark.uniqueId());
}

QString BrowserBookmarksSection::displayTitle(const BrowserBookmark &bookmark) const {
  if (bookmark.title.empty()) return bookmark.host();
  return QString::fromStdString(bookmark.title);
}

QString BrowserBookmarksSection::displaySubtitle(const BrowserBookmark &bookmark) const {
  return bookmark.host();
}

std::optional<ImageURL> BrowserBookmarksSection::displayIcon(const BrowserBookmark &bookmark) const {
  return bookmark.icon();
}

std::unique_ptr<ActionPanelState>
BrowserBookmarksSection::buildActionPanel(const BrowserBookmark &bookmark) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();
  const QString url = QString::fromStdString(bookmark.url);
  const QString title = QString::fromStdString(bookmark.title);

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
      new StaticAction(tr("Delete Bookmark"), BuiltinIcon::Trash, [bookmark](ApplicationContext *ctx) {
        ctx->navigation->confirmAlert(
            tr("Delete bookmark?"),
            tr("\"%1\" will be removed from your browser. This cannot be undone.")
                .arg(QString::fromStdString(bookmark.title.empty() ? bookmark.url : bookmark.title)),
            [ctx, bookmark]() {
              if (auto result = ctx->services->browserExtension()->removeBookmark(bookmark); !result) {
                ctx->services->toastService()->failure(
                    tr("Failed to delete bookmark: %1").arg(QString::fromStdString(result.error())));
                return;
              }
              ctx->services->toastService()->success(tr("Bookmark deleted"));
            });
      });
  remove->setShortcut(Keybind::DangerousRemoveAction);

  section->addAction(open);
  section->addAction(toggleDetail);
  section->addAction(copyUrl);
  section->addAction(copyTitle);
  section->addAction(convertToShortcut);

  auto dangerSection = panel->createSection();
  dangerSection->addAction(remove);

  return panel;
}
