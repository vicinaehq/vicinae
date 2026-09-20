#pragma once
#include "command/command-database.hpp"
#include "command/preference.hpp"
#include "services/files-service/file-service.hpp"
#include "builtins/file/search-files-view-host.hpp"
#include "command/single-view-command-context.hpp"
#include "command/typed-command.hpp"
#include "services/files-service/file-preferences.hpp"
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
    return ImageURL::builtin(BuiltinIcon::MagnifyingGlass).setBackgroundTint(SemanticColor::Yellow);
  }
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
    return ImageURL::builtin(BuiltinIcon::Hammer).setBackgroundTint(SemanticColor::Yellow);
  }

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

template <> struct PreferenceSchema<FilePreferences> {
#if defined(Q_OS_LINUX)
  PreferenceMeta autoIndexing{
      .title = tr("Enabled"),
      .description =
          tr("Whether to run the file indexer in the background. When turned off, the indexer process is "
             "stopped entirely and file search becomes unavailable until it is turned back on."),
  };
  PreferenceMeta indexingPaths{
      .title = tr("Search paths"),
      .description = tr("Directories that Vicinae will search"),
      .kind = PreferenceMeta::Kind::Directories,
  };
  PreferenceMeta excludedIndexingPaths{
      .title = tr("Excluded search paths"),
      .description = tr("Directories to exclude from file indexing"),
      .kind = PreferenceMeta::Kind::Directories,
  };
#elif defined(Q_OS_WIN)
  PreferenceMeta searchBackend{
      .title = tr("Search backend"),
      .description =
          tr("Automatic uses Everything when it is running and falls back to Windows Search otherwise."),
      .options =
          [] {
            return std::vector<Preference::DropdownData::Option>{
                option(FileSearchBackend::Auto, tr("Automatic")),
                option(FileSearchBackend::WindowsSearch, tr("Windows Search")),
                option(FileSearchBackend::Everything, tr("Everything")),
            };
          },
  };
  PreferenceMeta everythingInstance{
      .title = tr("Everything instance"),
      .description = tr("Name of the Everything instance to connect to. Leave empty for the default "
                        "instance, the Everything 1.5 alpha runs as \"1.5a\"."),
      .required = false,
  };
#endif
  Q_DECLARE_TR_FUNCTIONS(FilePreferences)
};

class FileExtension : public TypedCommandRepository<FilePreferences> {
  Q_DECLARE_TR_FUNCTIONS(FileExtension)

  QString id() const override { return "files"; }
  QString displayName() const override { return tr("System files"); }
  QString description() const override { return tr("Integrate with system files"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::MagnifyingGlass).setBackgroundTint(SemanticColor::Yellow);
  }

public:
  void initialized(const FilePreferences &preferences) const override {
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    ServiceRegistry::instance()->fileService()->preferencesChanged(preferences);
#endif
  }

  FileExtension() {
    registerCommand<SearchFilesCommand>();

    // XXX - we don't really need this anymore, as the indexer now executes full sweeps at a given interval
    // This behavior can be easily replicated by simply removing the `.cache/vicinae/file-indexer` directory.
    // registerCommand<RebuildFileIndexCommand>();
  }

  void preferencesChanged(const FilePreferences &preferences) const override {
#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    ServiceRegistry::instance()->fileService()->preferencesChanged(preferences);
#endif
  }
};
