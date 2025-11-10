#pragma once
#include "command-database.hpp"
#include "../../ui/image/url.hpp"
#include "common.hpp"
#include "preference.hpp"
#include "search-files-view.hpp"
#include "ui/alert/alert.hpp"
#include "utils.hpp"
#include "vicinae.hpp"
#include "services/toast/toast-service.hpp"

class SearchFilesCommand : public BuiltinViewCommand<SearchFilesView> {
  QString id() const override { return "search"; }
  QString name() const override { return "Search Files"; }
  QString description() const override { return "Search files on your system"; }
  bool isFallback() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("magnifying-glass").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<Preference> preferences() const override { return {}; }
  void preferenceValuesChanged(const QJsonObject &value) const override {}
};

class RebuildFileIndexCommand : public BuiltinCallbackCommand {
  QString id() const override { return "rebuild-index"; }
  QString name() const override { return "Rebuild File Index"; }
  QString description() const override {
    return "Fully rebuild the file index. Running this manually can be useful if the file search feels "
           "particularly out of date.";
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("hammer").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<Preference> preferences() const override { return {}; }
  void preferenceValuesChanged(const QJsonObject &value) const override {}

  void execute(CommandController *controller) const override {
    auto alert = new CallbackAlertWidget;
    auto ctx = controller->context();

    alert->setTitle("Are you sure?");
    alert->setMessage("Rebuilding the entire index can be time consuming and CPU intensive, depending on the "
                      "number of files present in your home directory.");
    alert->setConfirmText("Reset", SemanticColor::Red);
    alert->setCallback([ctx](bool confirmed) {
      if (!confirmed) return;

      ctx->services->fileService()->rebuildIndex();
      ctx->services->toastService()->setToast("Index rebuild started...");
    });
    ctx->navigation->setDialog(alert);
  }
};

class FileExtension : public BuiltinCommandRepository {
  QString id() const override { return "file"; }
  QString displayName() const override { return "System files"; }
  QString description() const override { return "Integrate with system files"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("magnifying-glass").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  void initialized(const QJsonObject &preferences) const override {
    auto files = ServiceRegistry::instance()->fileService();
    if (preferences.value("autoIndexing").toBool()) { files->indexer()->start(); }
  }

  FileExtension() {
    registerCommand<SearchFilesCommand>();
    registerCommand<RebuildFileIndexCommand>();
  }

  std::vector<Preference> preferences() const override {
    auto indexing = Preference::makeCheckbox("autoIndexing");

    indexing.setTitle("Auto Indexing");
    indexing.setDescription(
        "Whether to enable automatic file indexing in the background. If this is turned off, Vicinae will "
        "still be "
        "able to query the index if there is one, but will no longer update it by itself. This does not "
        "cancel ongoing indexing tasks.");
    indexing.setDefaultValue(true);

    auto paths = Preference::makeText("paths");
    paths.setTitle("Search paths");
    paths.setDescription("Semicolon-separated list of paths that vicinae will search");
    paths.setDefaultValue(homeDir().c_str());

    auto excludedPaths = Preference::makeText("excludedPaths");
    excludedPaths.setTitle("Excluded search paths");
    excludedPaths.setDescription("Semicolon-separated list of paths to exclude from file indexing");
    excludedPaths.setDefaultValue("");

    auto watcherPaths = Preference::makeText("watcherPaths");
    watcherPaths.setTitle("Watcher paths");
    watcherPaths.setDescription("Semicolon-separated list of paths watched by experimental watcher");
    watcherPaths.setDefaultValue("");

    return {indexing, paths, excludedPaths, watcherPaths};
  }

  void preferenceValuesChanged(const QJsonObject &preferences) const override {
    QStringList searchPaths = preferences.value("paths").toString().split(';', Qt::SkipEmptyParts);
    ServiceRegistry::instance()->fileService()->preferenceValuesChanged(preferences);
  }
};
