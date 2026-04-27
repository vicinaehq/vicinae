#pragma once
#include <qdatetime.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qmimedatabase.h>
#include <qurl.h>
#include <filesystem>
#include <optional>

#include "builtin_icon.hpp"
#include "clipboard-actions.hpp"
#include "qml/edit-keywords-view-host.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/alert/alert.hpp"
#include "utils/utils.hpp"

class PasteClipboardSelection : public PasteToFocusedWindowAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    setConcealed();
    loadClipboardData(Clipboard::SelectionRecordHandle(m_id));
    PasteToFocusedWindowAction::execute(ctx);
  }

public:
  PasteClipboardSelection(const QString &id) : PasteToFocusedWindowAction(), m_id(id) {}
};

class CopyClipboardSelection : public AbstractAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();
    if (clipman->copySelectionRecord(m_id, {.concealed = true})) {
      ctx->navigation->showHud("Selection copied to clipboard");
      return;
    }
    toast->failure("Failed to copy to clipboard");
  }

public:
  CopyClipboardSelection(const QString &id)
      : AbstractAction("Copy to clipboard", BuiltinIcon::CopyClipboard), m_id(id) {}
};

class SaveClipboardSelectionToFileAction : public AbstractAction {
  ClipboardHistoryEntry m_entry;
  QString m_saveDirectoryMode;
  QString m_saveCustomDirectory;
  QString m_saveFileNameMode;

  QString kindToken() const {
    switch (m_entry.kind) {
    case ClipboardOfferKind::Image:
      return "image";
    case ClipboardOfferKind::File:
      return "file";
    case ClipboardOfferKind::Link:
      return "link";
    case ClipboardOfferKind::Text:
      return "text";
    default:
      return "entry";
    }
  }

  static QStringList uriLines(const QByteArray &data) {
    QString const text = QString::fromUtf8(data);
    auto lines = text.split("\r\n", Qt::SkipEmptyParts);
    if (lines.empty()) lines = text.split('\n', Qt::SkipEmptyParts);
    return lines;
  }

  static std::optional<QFileInfo> singleLocalFileFromUriList(const QByteArray &data) {
    auto lines = uriLines(data);
    if (lines.size() != 1) return std::nullopt;

    QUrl const url(lines.front().trimmed());
    if (!url.isLocalFile()) return std::nullopt;

    QFileInfo const info(url.toLocalFile());
    if (!info.exists() || !info.isFile()) return std::nullopt;
    return info;
  }

  static QString sanitizeFileStem(const QString &input) {
    QString stem = slugify(input.simplified());
    if (stem.isEmpty()) stem = "clipboard-entry";
    if (stem.size() > 64) stem = stem.first(64);
    return stem;
  }

  static std::optional<QString> fileStemFromUriList(const QByteArray &data) {
    auto lines = uriLines(data);
    if (lines.empty()) return std::nullopt;

    if (auto info = singleLocalFileFromUriList(data); info) {
      QString stem = info->completeBaseName();
      if (stem.isEmpty()) stem = info->fileName();
      if (!stem.isEmpty()) return sanitizeFileStem(stem);
    }

    return QString("clipboard-files-%1").arg(lines.size());
  }

  QString defaultFileSuffix(const QByteArray &data) const {
    QString const mimeName = Utils::normalizeMimeName(m_entry.mimeType);
    if (mimeName == "text/uri-list") {
      if (auto info = singleLocalFileFromUriList(data); info) {
        QString const ext = info->suffix();
        if (!ext.isEmpty()) return ext;

        QMimeType const fileMime = QMimeDatabase().mimeTypeForFile(*info);
        if (fileMime.isValid()) {
          QString const preferred = fileMime.preferredSuffix();
          if (!preferred.isEmpty()) return preferred;
        }
      }
      return "txt";
    }

    QMimeType const mime = QMimeDatabase().mimeTypeForName(mimeName);
    if (mime.isValid()) {
      QString const preferred = mime.preferredSuffix();
      if (!preferred.isEmpty()) return preferred;

      auto const suffixes = mime.suffixes();
      if (!suffixes.isEmpty()) return suffixes.front();
    }

    if (Utils::isTextMimeType(mimeName)) return "txt";
    return "bin";
  }

  QString defaultFileStem(const QByteArray &data) const {
    if (m_saveFileNameMode == "timestamp") {
      auto dt = QDateTime::fromSecsSinceEpoch(m_entry.updatedAt);
      return QString("clipboard-%1-%2").arg(kindToken()).arg(dt.toString("yyyyMMdd-HHmmss"));
    }

    if (m_entry.kind == ClipboardOfferKind::Image) {
      auto dt = QDateTime::fromSecsSinceEpoch(m_entry.updatedAt);
      return QString("clipboard-image-%1").arg(dt.toString("yyyyMMdd-HHmmss"));
    }

    if (m_entry.kind == ClipboardOfferKind::File) {
      if (auto stem = fileStemFromUriList(data); stem) return *stem;
    }

    QString prefix;
    if (Utils::isTextMimeType(m_entry.mimeType) || m_entry.mimeType == "text/uri-list") {
      prefix = QString::fromUtf8(data.left(240)).simplified();
    }
    if (prefix.isEmpty()) prefix = m_entry.textPreview.simplified();
    if (prefix.isEmpty()) prefix = "clipboard-entry";

    return sanitizeFileStem(prefix.left(64));
  }

  QString defaultFileName(const QByteArray &data) const {
    auto stem = defaultFileStem(data);
    auto suffix = defaultFileSuffix(data);
    if (suffix.isEmpty()) return stem;
    return QString("%1.%2").arg(stem).arg(suffix);
  }

  QString saveDirectoryPath() const {
    if (m_saveDirectoryMode == "custom") {
      QString const custom = m_saveCustomDirectory.trimmed();
      if (!custom.isEmpty()) return custom;
    }

    if (m_saveDirectoryMode == "home") {
      auto folder = homeDir();
      if (!folder.empty()) return QString::fromStdString(folder.string());
      return QDir::homePath();
    }

    auto folder = downloadsFolder();
    std::error_code ec;
    if (folder.empty() || !std::filesystem::is_directory(folder, ec)) { folder = homeDir(); }
    if (folder.empty()) return QDir::homePath();
    return QString::fromStdString(folder.string());
  }

  QString uniqueTargetPath(const QString &defaultName) const {
    QDir dir(saveDirectoryPath());
    if (!dir.exists()) {
      if (!dir.mkpath(".")) return QString();
    }

    QString candidate = dir.filePath(defaultName);
    if (!QFileInfo::exists(candidate)) return candidate;

    QFileInfo const info(defaultName);
    QString const base = info.completeBaseName();
    QString const suffix = info.suffix();

    for (int i = 2; i < 10'000; ++i) {
      QString numbered = QString("%1-%2").arg(base).arg(i);
      if (!suffix.isEmpty()) numbered += QString(".%1").arg(suffix);

      candidate = dir.filePath(numbered);
      if (!QFileInfo::exists(candidate)) return candidate;
    }

    return QString();
  }

  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();

    auto data = clipman->getMainOfferData(m_entry.id);
    if (!data) {
      toast->failure("Failed to read clipboard entry");
      return;
    }

    QByteArray const bytes = data.value();
    QString const path = uniqueTargetPath(defaultFileName(bytes));
    if (path.isEmpty()) {
      toast->failure("Failed to resolve output file path");
      return;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
      toast->failure("Failed to save file", file.errorString());
      return;
    }

    if (file.write(bytes) != bytes.size()) {
      toast->failure("Failed to write file", file.errorString());
      return;
    }

    file.close();

    toast->success(QString("Saved to %1").arg(path));
  }

public:
  SaveClipboardSelectionToFileAction(const ClipboardHistoryEntry &entry, QString saveDirectoryMode,
                                     QString saveCustomDirectory, QString saveFileNameMode)
      : AbstractAction("Save to file", BuiltinIcon::SaveDocument), m_entry(entry),
        m_saveDirectoryMode(std::move(saveDirectoryMode)),
        m_saveCustomDirectory(std::move(saveCustomDirectory)),
        m_saveFileNameMode(std::move(saveFileNameMode)) {}
};

class RemoveSelectionAction : public AbstractAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();
    if (clipman->removeSelection(m_id)) {
      toast->setToast("Entry removed");
    } else {
      toast->setToast("Failed to remove entry", ToastStyle::Danger);
    }
  }

public:
  RemoveSelectionAction(const QString &id)
      : AbstractAction("Remove entry", ImageURL::builtin("trash")), m_id(id) {
    setStyle(AbstractAction::Style::Danger);
  }
};

class PinClipboardAction : public AbstractAction {
  QString m_id;
  bool m_value;
  void execute(ApplicationContext *ctx) override {
    QString action = m_value ? "pinned" : "unpinned";
    if (ctx->services->clipman()->setPinned(m_id, m_value)) {
      ctx->services->toastService()->success(QString("Selection %1").arg(action));
    } else {
      ctx->services->toastService()->failure("Failed to change pin status");
    }
  }

public:
  PinClipboardAction(const QString &id, bool value)
      : AbstractAction(value ? "Pin" : "Unpin", ImageURL::builtin("pin")), m_id(id), m_value(value) {}
};

class EditClipboardKeywordsAction : public AbstractAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto id = m_id;
    auto view =
        new EditKeywordsViewHost([clipman, id]() { return clipman->retrieveKeywords(id).value_or(""); },
                                 [clipman, id](const QString &kw) { return clipman->setKeywords(id, kw); },
                                 "Additional keywords that will be used to index this selection.");
    ctx->navigation->pushView(view);
    ctx->navigation->setNavigationTitle(title());
  }

public:
  QString title() const override { return "Edit keywords"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("text"); }
  EditClipboardKeywordsAction(const QString &id) : m_id(id) {}
};

class RemoveAllSelectionsAction : public AbstractAction {
  void execute(ApplicationContext *ctx) override {
    auto alert = new CallbackAlertWidget();
    alert->setTitle("Are you sure?");
    alert->setMessage("All your clipboard history will be lost forever");
    alert->setConfirmText("Delete all", SemanticColor::Red);
    alert->setConfirmCallback([ctx]() {
      auto toast = ctx->services->toastService();
      auto clipman = ctx->services->clipman();
      if (clipman->removeAllSelections()) {
        toast->success("All selections were removed");
      } else {
        toast->failure("Failed to remove all selections");
      }
    });
    ctx->navigation->setDialog(alert);
  }

public:
  QString title() const override { return "Remove all"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("trash"); }
  RemoveAllSelectionsAction() { setStyle(AbstractAction::Style::Danger); }
};
