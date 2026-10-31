#pragma once
#include <QCoreApplication>
#include <QFile>
#include "actions/app-actions.hpp"
#include "actions/file-actions.hpp"
#include "actions/shortcut-actions.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "actions/clipboard-actions.hpp"
#include "common/context.hpp"
#include "keyboard/keybind.hpp"
#include "keyboard/keyboard.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "internal/keyboard/keyboard.hpp"
#include "services/toast/toast-service.hpp"
#include "services/wallpaper/wallpaper-manager.hpp"
#include "ui/action-panel/action.hpp"
#include <algorithm>
#include <qmimedatabase.h>
#include <filesystem>
#include <memory>
#include <system_error>

namespace FileActions {

class RevealFileInFolderAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(RevealFileInFolderAction)

public:
  RevealFileInFolderAction(std::filesystem::path path)
      : AbstractAction(tr("Show in file browser"), ImageURL::builtin(BuiltinIcon::Folder)),
        m_path(std::move(path)) {
    setShortcut(Keyboard::Shortcut::submit());
  }

  void execute(ApplicationContext *ctx) override {
    auto const appDb = ctx->services->appDb();
    auto const toast = ctx->services->toastService();

    bool const success = appDb->showInFileBrowser(m_path, true);

    if (!success) {
      toast->failure(tr("Failed to open folder"));
      return;
    }

    ctx->navigation->closeWindow();
  }

private:
  std::filesystem::path m_path;
};

class SetWallpaperAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(SetWallpaperAction)

public:
  SetWallpaperAction(std::filesystem::path path)
      : AbstractAction(tr("Set as wallpaper"), ImageURL::builtin(BuiltinIcon::Image)),
        m_path(std::move(path)) {
    setShortcut(Keyboard::Shortcut(Qt::Key_W, Qt::ControlModifier | Qt::ShiftModifier));
  }

  void execute(ApplicationContext *ctx) override {
    auto const toast = ctx->services->toastService();
    auto const wallpaper = ctx->services->wallpaperManager();

    wallpaper->setWallpaper({.path = m_path.string()})
        .then(toast, [ctx, toast](const std::expected<void, std::string> &result) {
          if (result) {
            ctx->navigation->showHud(tr("Wallpaper set"), ImageURL::builtin(BuiltinIcon::Image));
          } else {
            toast->failure(tr("Failed to set wallpaper"), QString::fromStdString(result.error()));
          }
        });
  }

private:
  std::filesystem::path m_path;
};

class RunExecutableAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(RunExecutableAction)

public:
  struct Options {
    bool mkExec = false;
  };

  RunExecutableAction(std::filesystem::path path, const Options &opts)
      : m_path(std::move(path)), m_opts(opts) {}

  QString title() const override { return tr("Run executable"); }

  std::optional<ImageURL> icon() const override { return BuiltinIcon::Terminal; }

  void execute(ApplicationContext *ctx) override {
    namespace fs = std::filesystem;
    auto const files = ctx->services->fileService();

    if (m_opts.mkExec) {
      std::error_code ec{};
      fs::permissions(m_path, fs::perms::owner_exec, fs::perm_options::add, ec);
      if (ec) {
        ctx->services->toastService()->failure(tr("Failed to give executable permission"));
        return;
      }
    }

    if (ctx->services->appDb()->launchRaw({QString::fromStdString(m_path.string())})) {
      ctx->navigation->closeWindow();
      files->recordAccess(m_path);
    } else {
      ctx->services->toastService()->failure(tr("Failed to start executable"));
    }
  }

private:
  std::filesystem::path m_path;
  Options m_opts;
};

class OpenTerminalAtPathAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(OpenTerminalAtPathAction)

public:
  OpenTerminalAtPathAction(std::filesystem::path path)
      : AbstractAction(tr("Open in Terminal"), ImageURL::builtin(BuiltinIcon::Terminal)),
        m_path(std::move(path)) {
    setShortcut(Keyboard::Shortcut(Qt::Key_T, Qt::ControlModifier));
  }

  void execute(ApplicationContext *ctx) override {
    auto const appDb = ctx->services->appDb();
    auto const toast = ctx->services->toastService();

    std::error_code ec;
    std::filesystem::path dir = std::filesystem::is_directory(m_path, ec) ? m_path : m_path.parent_path();
    QString dirStr = QString::fromStdString(dir.string());

    LaunchTerminalCommandOptions opts;
    opts.workingDirectory = dirStr;

    bool ok = appDb->launchTerminalCommand({}, opts);
    if (!ok) {
      ok = appDb->launchTerminalCommand({qEnvironmentVariable("SHELL", QStringLiteral("/bin/sh"))}, opts);
    }

    if (!ok) {
      toast->failure(tr("Failed to open terminal at path"));
      return;
    }

    ctx->navigation->closeWindow();
  }

private:
  std::filesystem::path m_path;
};

class CopyParentDirectoryPathAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(CopyParentDirectoryPathAction)

public:
  CopyParentDirectoryPathAction(std::filesystem::path path)
      : AbstractAction(tr("Copy Folder Path"), ImageURL::builtin(BuiltinIcon::Folder)),
        m_path(std::move(path)) {}

  void execute(ApplicationContext *ctx) override {
    auto const clipman = ctx->services->clipman();
    auto const toast = ctx->services->toastService();
    QString parent = QString::fromStdString(m_path.parent_path().string());

    if (clipman->copyText(parent)) {
      ctx->navigation->showHud(tr("Folder path copied"), ImageURL::builtin(BuiltinIcon::CopyClipboard));
    } else {
      toast->failure(tr("Failed to copy folder path"));
    }
  }

private:
  std::filesystem::path m_path;
};

class CopyFileContentAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(CopyFileContentAction)

public:
  CopyFileContentAction(std::filesystem::path path)
      : AbstractAction(tr("Copy File Content"), ImageURL::builtin(BuiltinIcon::CopyClipboard)),
        m_path(std::move(path)) {}

  void execute(ApplicationContext *ctx) override {
    auto const toast = ctx->services->toastService();
    auto const clipman = ctx->services->clipman();

    std::error_code ec;
    auto size = std::filesystem::file_size(m_path, ec);
    if (ec || size > 2 * 1024 * 1024) {
      toast->failure(tr("File too large to copy content"));
      return;
    }

    QFile file(QString::fromStdString(m_path.string()));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      toast->failure(tr("Failed to read file"));
      return;
    }

    QString content = QString::fromUtf8(file.readAll());
    if (clipman->copyText(content)) {
      ctx->navigation->showHud(tr("File content copied"), ImageURL::builtin(BuiltinIcon::CopyClipboard));
    } else {
      toast->failure(tr("Failed to copy file content"));
    }
  }

private:
  std::filesystem::path m_path;
};

class MoveFileToTrashAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(MoveFileToTrashAction)

public:
  MoveFileToTrashAction(std::filesystem::path path)
      : AbstractAction(tr("Move to Trash"), ImageURL::builtin(BuiltinIcon::Trash)), m_path(std::move(path)) {
    setShortcut(Keyboard::Shortcut(Qt::Key_Backspace, Qt::ControlModifier));
  }

  void execute(ApplicationContext *ctx) override {
    auto const toast = ctx->services->toastService();
    QString const qPath = QString::fromStdString(m_path.string());

    if (QFile::moveToTrash(qPath)) {
      ctx->navigation->showHud(tr("Moved to trash"), ImageURL::builtin(BuiltinIcon::Trash));
    } else {
      toast->failure(tr("Failed to move file to trash"));
    }
  }

private:
  std::filesystem::path m_path;
};

inline std::unique_ptr<ActionPanelState> actionPanel(const std::filesystem::path &path,
                                                     const ApplicationContext *ctx) {
  // extensions for which we allow granting executable permission on the fly
  // most of the time we want to avoid doing that, but for e.g AppImages that's
  // what a lot of users would expect.
  const auto AUTO_EXECUTABLE_EXTENSIONS = {".AppImage"};

  QMimeDatabase mimeDb;
  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();
  auto mime = mimeDb.mimeTypeForFile(QString::fromStdString(path.string()));
  auto appDb = ctx->services->appDb();
  auto pasteService = ctx->services->pasteService();
  auto openers = appDb->findCuratedOpeners(QString::fromStdString(path.string()));
  auto fileBrowser = appDb->fileBrowser();

  if (!openers.empty()) {
    auto open = new OpenFileAction(path, openers.front());
    section->addAction(open);
  }

  {
    AbstractAction *action = nullptr;

    if (std::ranges::any_of(AUTO_EXECUTABLE_EXTENSIONS,
                            [&](auto &&ext) { return path.extension() == ext; })) {
      action = new RunExecutableAction(path, {.mkExec = true});
    }

    if (action) {
      if (openers.empty()) action->setPrimary(true);
      section->addAction(action);
    }
  }

  if (fileBrowser) { section->addAction(new RevealFileInFolderAction(path)); }

  section->addAction(new OpenTerminalAtPathAction(path));

  section->addAction(new OpenWithAction(QString::fromStdString(path.string())));

  if (mime.name().startsWith("image/") && ctx->services->wallpaperManager()->canSetWallpaper()) {
    section->addAction(new SetWallpaperAction(path));
  }

  section->addAction(new CreateShortcutAction({
      .link = QString::fromStdString(path.string()),
      .name = QString::fromStdString(path.filename().string()),
  }));

  auto utils = panel->createSection();
  auto copy = AbstractAction::make<CopyToClipboardAction>(
      Clipboard::File(path), QCoreApplication::translate("file-list-item", "Copy file"));
  auto copyPath = AbstractAction::make<CopyToClipboardAction>(
      Clipboard::Text(QString::fromStdString(path.string())),
      QCoreApplication::translate("file-list-item", "Copy file path"));
  auto copyFolder = new CopyParentDirectoryPathAction(path);
  auto copyFileName = AbstractAction::make<CopyToClipboardAction>(
      Clipboard::Text(QString::fromStdString(path.filename().string())),
      QCoreApplication::translate("file-list-item", "Copy file name"));

  copy->setShortcut(Keybind::CopyAction);
  copyPath->setShortcut(Keyboard::Shortcut(Qt::Key_C, Qt::ControlModifier | Qt::ShiftModifier));

  if (pasteService->supportsPaste()) {
    auto paste = AbstractAction::make<PasteToFocusedWindowAction>(Clipboard::File(path));
    paste->setShortcut(Keyboard::Shortcut::osPaste().shifted());
    utils->addAction(std::move(paste));
  }

  utils->addAction(std::move(copy));
  utils->addAction(std::move(copyPath));
  utils->addAction(copyFolder);
  utils->addAction(std::move(copyFileName));

  if (mime.name().startsWith(QStringLiteral("text/")) || mime.name() == QStringLiteral("application/json") ||
      mime.name() == QStringLiteral("application/xml") ||
      mime.name() == QStringLiteral("application/javascript")) {
    utils->addAction(new CopyFileContentAction(path));
  }

  if (mime.isValid()) {
    utils->addAction(new CopyToClipboardAction(
        Clipboard::Text(mime.name()), QCoreApplication::translate("file-list-item", "Copy mime type")));
  }

  auto dangerSection = panel->createSection();
  dangerSection->addAction(new MoveFileToTrashAction(path));

  return panel;
}

} // namespace FileActions
