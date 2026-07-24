#pragma once
#include "command-database.hpp"
#include "preference.hpp"
#include "services/files-service/file-service.hpp"
#include "qml/search-files-view-host.hpp"
#include "single-view-command-context.hpp"
#include "ui/alert/alert.hpp"
#include "utils.hpp"
#include "vicinae.hpp"
#include "services/toast/toast-service.hpp"
#include <QCoreApplication>

class SearchFilesCommand : public BuiltinViewCommand<SearchFilesViewHost> {
  Q_DECLARE_TR_FUNCTIONS(SearchFilesCommand)

  QString id() const override { return "search"; }
  QString name() const override { return tr("Search Files"); }
  QString description() const override { return tr("Search files on your system"); }
  bool isFallback() const override { return true; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("magnifying-glass").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<Preference> preferences() const override { return {}; }
  void preferenceValuesChanged(const QJsonObject &value) const override {}
};

class RebuildFileIndexCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(RebuildFileIndexCommand)

  QString id() const override { return "rebuild-index"; }
  QString name() const override { return tr("Rebuild File Index"); }
  QString description() const override {
    return tr("Fully rebuild the file index. Running this manually can be useful if the file search feels "
              "particularly out of date.");
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("hammer").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<Preference> preferences() const override { return {}; }
  void preferenceValuesChanged(const QJsonObject &value) const override {}

  void execute(CommandController &controller) const override {
    auto alert = new CallbackAlertWidget;
    auto ctx = controller.context();

    alert->setTitle(tr("Are you sure?"));
    alert->setMessage(
        tr("Rebuilding the entire index can be time consuming and CPU intensive, depending on the "
           "number of files present in your home directory."));
    alert->setConfirmText(tr("Reset"), SemanticColor::Red);
    alert->setCallback([ctx](bool confirmed) {
      if (!confirmed) return;

      ctx->services->fileService()->rebuildIndex();
      ctx->services->toastService()->setToast(tr("Index rebuild started..."));
    });
    ctx->navigation->setDialog(alert);
  }
};

class FileExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(FileExtension)

  QString id() const override { return "files"; }
  QString displayName() const override { return tr("System files"); }
  QString description() const override { return tr("Integrate with system files"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("magnifying-glass").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  void initialized(const QJsonObject &preferences) const override {
#ifdef Q_OS_LINUX
    ServiceRegistry::instance()->fileService()->preferenceValuesChanged(preferences);
#endif
  }

  FileExtension() {
    registerCommand<SearchFilesCommand>();

    // XXX - we don't really need this anymore, as the indexer now executes full sweeps at a given interval
    // This behavior can be easily replicated by simply removing the `.cache/vicinae/file-indexer` directory.
    // registerCommand<RebuildFileIndexCommand>();
  }

  std::vector<Preference> preferences() const override {
#ifdef Q_OS_LINUX
    auto indexing = Preference::makeCheckbox("autoIndexing");

    indexing.setTitle(tr("Enabled"));
    indexing.setDescription(
        tr("Whether to run the file indexer in the background. When turned off, the indexer process is "
           "stopped entirely and file search becomes unavailable until it is turned back on."));
    indexing.setDefaultValue(true);

    auto paths = Preference::directories("indexingPaths");
    paths.setTitle(tr("Search paths"));
    paths.setDescription(tr("Directories that Vicinae will search"));
    paths.setDefaultValue(QJsonArray{homeDir().c_str()});

    auto excludedPaths = Preference::directories("excludedIndexingPaths");
    excludedPaths.setTitle(tr("Excluded search paths"));
    excludedPaths.setDescription(tr("Directories to exclude from file indexing"));
    excludedPaths.setDefaultValue(QJsonArray{});

    return {indexing, paths, excludedPaths};
#else
    return {};
#endif
  }

  void preferenceValuesChanged(const QJsonObject &preferences) const override {
#ifdef Q_OS_LINUX
    ServiceRegistry::instance()->fileService()->preferenceValuesChanged(preferences);
#endif
  }
};
