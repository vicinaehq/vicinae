#pragma once

#include <QCoreApplication>
#include "builtins/screenshots/screenshot-actions.hpp"
#include "builtins/screenshots/screenshots-view-host.hpp"
#include "command/single-view-command-context.hpp"

class SearchScreenshotsCommand : public BuiltinViewCommand<ScreenshotsViewHost> {
  Q_DECLARE_TR_FUNCTIONS(SearchScreenshotsCommand)

public:
  QString id() const override { return "browse"; }
  QString name() const override { return tr("Search Screenshots"); }
  QString description() const override {
    return tr("Search and share saved screenshots and screen recordings.");
  }
  std::vector<QString> keywords() const override {
    return {"browse screenshots", "screen recordings", "video captures"};
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Screenshot).setBackgroundTint(SemanticColor::Blue);
  }
};

class PasteLastScreenshotCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(PasteLastScreenshotCommand)

public:
  QString id() const override { return "paste-last"; }
  QString name() const override { return tr("Paste Last Screenshot"); }
  QString description() const override {
    return tr("Paste the most recent saved screenshot into the active app.");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Screenshot)
        .setBackgroundTint(SemanticColor::Blue)
        .setBadge(BuiltinIcon::CopyClipboard);
  }
  void execute(CommandController &controller) const override {
    ScreenshotActions::pasteLast(controller.context());
  }
};

class ScreenshotsExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(ScreenshotsExtension)

public:
  QString id() const override { return "screenshots"; }
  QString displayName() const override { return tr("Screenshots"); }
  QString description() const override {
    return tr("Search and share saved screenshots and screen recordings.");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Screenshot).setBackgroundTint(SemanticColor::Blue);
  }

  ScreenshotsExtension() {
    registerCommand<SearchScreenshotsCommand>();
    registerCommand<PasteLastScreenshotCommand>();
  }
};
