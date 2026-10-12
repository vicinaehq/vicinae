#include "builtins/clipboard/history/clipboard-history-model.hpp"
#include "actions/app-actions.hpp"
#include "actions/save-as-action.hpp"
#include "common/context.hpp"
#include "builtins/clipboard/history/clipboard-history-actions.hpp"
#include "internal/keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "services/clipboard/clipboard-db.hpp"
#include "ui/settings/settings-controller.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/paste/paste-service.hpp"
#include "ui/action-panel/action.hpp"
#include "utils/utils.hpp"
#include <QCoreApplication>
#include <QDateTime>
#include <QMimeDatabase>
#include <filesystem>
#include <qlogging.h>
#include <qnamespace.h>

static std::string suggestedFileName(const ClipboardHistoryEntry &entry) {
  QMimeDatabase const db;
  auto suffix = db.mimeTypeForName(Utils::normalizeMimeName(entry.mimeType)).preferredSuffix();

  if (suffix.isEmpty() && entry.kind != ClipboardOfferKind::Image) suffix = QStringLiteral("txt");

  auto name = QStringLiteral("clipboard-") +
              QDateTime::fromSecsSinceEpoch(entry.updatedAt).toString(QStringLiteral("yyyyMMdd-HHmmss"));

  if (!suffix.isEmpty()) name += u'.' + suffix;

  return name.toStdString();
}

static SaveAsAction *saveSelectionAction(ClipboardService *clipman, const ClipboardHistoryEntry &entry) {
  auto save =
      new SaveAsAction([clipman, id = entry.id,
                        name = suggestedFileName(entry)]() -> std::expected<SaveAsAction::Payload, QString> {
        auto data = clipman->getMainOfferData(id);
        if (!data) {
          return std::unexpected(
              QCoreApplication::translate("ClipboardHistorySection", "Failed to read clipboard data"));
        }
        return SaveAsAction::Payload{.suggestedName = name, .content = std::move(*data)};
      });
  save->setShortcut(Keybind::SaveAction);
  return save;
}

static SaveAsAction *saveFileAction(std::filesystem::path path) {
  auto save = new SaveAsAction([path = std::move(path)]() -> std::expected<SaveAsAction::Payload, QString> {
    return SaveAsAction::Payload{.suggestedName = path.filename().string(), .content = path};
  });
  save->setShortcut(Keybind::SaveAction);
  return save;
}

void ClipboardHistorySection::setEntries(const PaginatedResponse<ClipboardHistoryEntry> &page) {
  m_entries = page.data;
  notifyChanged();
}

QString ClipboardHistorySection::itemId(int i) const { return m_entries[i].id; }

QString ClipboardHistorySection::itemTitle(int i) const { return m_entries[i].textPreview; }

QString ClipboardHistorySection::itemSubtitle(int i) const {
  auto dt = QDateTime::fromSecsSinceEpoch(m_entries[i].updatedAt);
  return getRelativeTimeString(dt);
}

std::optional<ImageURL> ClipboardHistorySection::itemIcon(int i) const { return iconForEntry(m_entries[i]); }

ImageURL ClipboardHistorySection::iconForEntry(const ClipboardHistoryEntry &entry) const {
  switch (entry.kind) {
  case ClipboardOfferKind::Image:
    return ImageURL::builtin(BuiltinIcon::Image);
  case ClipboardOfferKind::Link:
    if (entry.urlHost)
      return ImageURL::favicon(*entry.urlHost).withFallback(ImageURL::builtin(BuiltinIcon::Link));
    return ImageURL::builtin(BuiltinIcon::Link);
  case ClipboardOfferKind::Text:
    return ImageURL::builtin(BuiltinIcon::Text);
  case ClipboardOfferKind::File:
    return ImageURL::builtin(BuiltinIcon::Folder);
  default:
    return ImageURL::builtin(BuiltinIcon::QuestionMarkCircle);
  }
}

bool ClipboardHistorySection::isDraggable(int idx) const { return true; }

std::unique_ptr<QMimeData> ClipboardHistorySection::dragMimeData(int idx) const {
  auto clipman = scope().services()->clipman();
  auto selection = clipman->retrieveSelectionById(m_entries[idx].id);

  if (!selection) return nullptr;

  return clipman->dragMimeDataForSelection(*std::move(selection));
}

std::unique_ptr<ActionPanelState> ClipboardHistorySection::actionPanel(int i) const {
  const auto &entry = m_entries[i];
  auto panel = std::make_unique<ListActionPanelState>();
  auto clipman = scope().services()->clipman();
  auto mainSection = panel->createSection();
  auto appDb = scope().services()->appDb();
  bool const isCopyable = entry.encryption == ClipboardEncryptionType::None || clipman->isEncryptionReady();

  if (!isCopyable) {
    mainSection->addAction(
        new StaticAction(QCoreApplication::translate("ClipboardHistorySection", "Open Settings"),
                         BuiltinIcon::Cog, [](ApplicationContext *ctx) {
                           ctx->settings->openTab(QStringLiteral("advanced"));
                           ctx->navigation->closeWindow();
                         }));
  }

  auto pasteService = scope().services()->pasteService();
  auto pin = new PinClipboardAction(entry.id, !entry.pinnedAt);
  auto editKeywords = new EditClipboardKeywordsAction(entry.id);
  auto remove = new RemoveSelectionAction(entry.id);
  auto removeAll = new RemoveAllSelectionsAction();

  editKeywords->setShortcut(Keybind::EditAction);
  remove->setStyle(AbstractAction::Style::Danger);
  remove->setShortcut(Keybind::RemoveAction);
  removeAll->setShortcut(Keybind::DangerousRemoveAction);
  pin->setShortcut(Keybind::PinAction);

  if (isCopyable) {
    auto copy = new CopyClipboardSelection(entry.id);
    copy->addShortcut(Keybind::CopyAction);

    if (pasteService->supportsPaste()) {
      auto paste = new PasteClipboardSelection(entry.id);
      paste->addShortcut(Keybind::PasteAction);
      if (m_defaultAction == DefaultAction::Copy) {
        mainSection->addAction(copy);
        mainSection->addAction(paste);
      } else {
        mainSection->addAction(paste);
        mainSection->addAction(copy);
      }
    } else {
      mainSection->addAction(copy);
    }
  }

  {
    if (entry.kind == ClipboardOfferKind::File) {
      if (const auto data = clipman->getMainOfferData(entry.id)) {
        auto urls = QString{*data}.split("\r\n", Qt::SkipEmptyParts);
        if (urls.size() == 1) {
          if (QUrl url{urls.front()}; QFile::exists(url.path())) {
            if (auto app = appDb->findDefaultOpener(url.path())) {
              auto open = new OpenAppAction(app, "Open", {url.path()});
              open->setShortcut(Keybind::OpenAction);
              mainSection->addAction(open);
            }

            auto openWith = new OpenWithAction(url.path());
            openWith->setShortcut(Keyboard::Shortcut(Qt::Key_O, Qt::ControlModifier).shifted());
            mainSection->addAction(openWith);

            std::error_code ec;
            std::filesystem::path path = url.toLocalFile().toStdString();
            if (std::filesystem::is_regular_file(path, ec)) {
              mainSection->addAction(saveFileAction(std::move(path)));
            }
          }
        }
      }
    }

    else if (entry.kind == ClipboardOfferKind::Link) {
      if (const auto data = clipman->getMainOfferData(entry.id)) {
        if (auto app = appDb->findDefaultOpener(*data)) {
          auto open = new OpenAppAction(app, "Open", {*data});
          open->setShortcut(Keybind::OpenAction);
          mainSection->addAction(open);
        }

        auto openWith = new OpenWithAction(*data);
        openWith->setShortcut(Keyboard::Shortcut(Qt::Key_O, Qt::ControlModifier).shifted());
        mainSection->addAction(openWith);
      }
    }

    else if (isCopyable) {
      const auto mime = Utils::normalizeMimeName(entry.mimeType);
      auto fileName = suggestedFileName(entry);

      if (auto app = appDb->findDefaultOpenerForMime(mime)) {
        auto open = new OpenClipboardSelectionAction(
            app, QCoreApplication::translate("ClipboardHistorySection", "Open"), entry.id, fileName);
        open->setShortcut(Keybind::OpenAction);
        mainSection->addAction(open);
      }

      auto openWith = new OpenClipboardSelectionWithAction(entry.id, mime, std::move(fileName));
      openWith->setShortcut(Keyboard::Shortcut(Qt::Key_O, Qt::ControlModifier).shifted());
      mainSection->addAction(openWith);
    }
  }

  if (isCopyable && entry.kind != ClipboardOfferKind::File) {
    mainSection->addAction(saveSelectionAction(clipman, entry));
  }

  auto toolsSection = panel->createSection();
  auto dangerSection = panel->createSection();
  toolsSection->addAction(pin);
  toolsSection->addAction(editKeywords);
  dangerSection->addAction(remove);
  dangerSection->addAction(removeAll);

  return panel;
}
