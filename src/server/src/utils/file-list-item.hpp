#pragma once
#include <QCoreApplication>
#include "actions/app/app-actions.hpp"
#include "actions/files/file-actions.hpp"
#include "builtin_icon.hpp"
#include "clipboard-actions.hpp"
#include "common/context.hpp"
#include "keyboard/keybind.hpp"
#include "keyboard/keyboard.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "internal/keyboard/keyboard.hpp"
#include "services/toast/toast-service.hpp"
#include "services/wallpaper/wallpaper-manager.hpp"
#include "ui/action-pannel/action.hpp"
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
    auto const files = ctx->services->fileService();
    auto const toast = ctx->services->toastService();

    bool const success = appDb->showInFileBrowser(m_path, true);

    if (!success) {
      toast->failure(tr("Failed to open folder"));
      return;
    }

    files->saveAccess(m_path);
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
      files->saveAccess(m_path);
    } else {
      ctx->services->toastService()->failure(tr("Failed to start executable"));
    }
  }

private:
  std::filesystem::path m_path;
  Options m_opts;
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

  section->addAction(new OpenWithAction(QString::fromStdString(path.string())));

  if (mime.name().startsWith("image/") && ctx->services->wallpaperManager()->canSetWallpaper()) {
    section->addAction(new SetWallpaperAction(path));
  }

  auto utils = panel->createSection();
  auto copy = AbstractAction::make<CopyToClipboardAction>(
      Clipboard::File(path), QCoreApplication::translate("file-list-item", "Copy file"));
  auto copyPath = AbstractAction::make<CopyToClipboardAction>(
      Clipboard::Text(QString::fromStdString(path.string())),
      QCoreApplication::translate("file-list-item", "Copy file path"));
  auto copyFileName = AbstractAction::make<CopyToClipboardAction>(
      Clipboard::Text(QString::fromStdString(path.filename().string())),
      QCoreApplication::translate("file-list-item", "Copy file name"));

  copy->setShortcut(Keybind::CopyAction);

  if (pasteService->supportsPaste()) {
    auto paste = AbstractAction::make<PasteToFocusedWindowAction>(Clipboard::File(path));
    paste->setShortcut(Keyboard::Shortcut::osPaste().shifted());
    utils->addAction(std::move(paste));
  }

  // TODO: for some reason those are broken, we need to investigate why
  // copyPath->setShortcut(Keybind::CopyPathAction);
  // copyFileName->setShortcut(Keybind::CopyNameAction);

  utils->addAction(std::move(copy));
  utils->addAction(std::move(copyPath));
  utils->addAction(std::move(copyFileName));

  if (mime.isValid()) {
    utils->addAction(new CopyToClipboardAction(
        Clipboard::Text(mime.name()), QCoreApplication::translate("file-list-item", "Copy mime type")));
  }

  return panel;
}

} // namespace FileActions
