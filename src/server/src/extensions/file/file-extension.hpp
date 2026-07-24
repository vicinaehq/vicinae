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

namespace {

ImageURL searchFilesIcon(vicinae::FileCategory category) {
  switch (category) {
  case vicinae::FileCategory::Directory:
    return ImageURL::builtin("folder").setBackgroundTint(Omnicast::ACCENT_COLOR);
  case vicinae::FileCategory::Image:
    return ImageURL::builtin("image").setBackgroundTint(Omnicast::ACCENT_COLOR);
  case vicinae::FileCategory::Video:
    return ImageURL::builtin("video").setBackgroundTint(Omnicast::ACCENT_COLOR);
  case vicinae::FileCategory::Audio:
    return ImageURL::builtin("headphones").setBackgroundTint(Omnicast::ACCENT_COLOR);
  case vicinae::FileCategory::Document:
    return ImageURL::builtin("blank-document").setBackgroundTint(Omnicast::ACCENT_COLOR);
  case vicinae::FileCategory::Archive:
    return ImageURL::builtin("box").setBackgroundTint(Omnicast::ACCENT_COLOR);
  case vicinae::FileCategory::Application:
    return ImageURL::builtin("app-window-sidebar-left").setBackgroundTint(Omnicast::ACCENT_COLOR);
  default:
    return ImageURL::builtin("magnifying-glass").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
}

template <vicinae::FileCategory Category> class SearchFilesCategoryCommand : public BuiltinViewCommand<SearchFilesViewHost> {
  QString id() const override {
    if constexpr (Category == vicinae::FileCategory::Directory) return "search-directories";
    if constexpr (Category == vicinae::FileCategory::Image) return "search-images";
    if constexpr (Category == vicinae::FileCategory::Video) return "search-videos";
    if constexpr (Category == vicinae::FileCategory::Audio) return "search-audio";
    if constexpr (Category == vicinae::FileCategory::Document) return "search-documents";
    if constexpr (Category == vicinae::FileCategory::Archive) return "search-archives";
    if constexpr (Category == vicinae::FileCategory::Application) return "search-applications";
    return "search-files";
  }

  QString name() const override {
    if constexpr (Category == vicinae::FileCategory::Directory) return "Search Directories";
    if constexpr (Category == vicinae::FileCategory::Image) return "Search Images";
    if constexpr (Category == vicinae::FileCategory::Video) return "Search Videos";
    if constexpr (Category == vicinae::FileCategory::Audio) return "Search Audio";
    if constexpr (Category == vicinae::FileCategory::Document) return "Search Documents";
    if constexpr (Category == vicinae::FileCategory::Archive) return "Search Archives";
    if constexpr (Category == vicinae::FileCategory::Application) return "Search Applications";
    return "Search Files";
  }

  QString description() const override {
    if constexpr (Category == vicinae::FileCategory::Directory) return "Search directories on your system";
    if constexpr (Category == vicinae::FileCategory::Image) return "Search image files on your system";
    if constexpr (Category == vicinae::FileCategory::Video) return "Search video files on your system";
    if constexpr (Category == vicinae::FileCategory::Audio) return "Search audio files on your system";
    if constexpr (Category == vicinae::FileCategory::Document) return "Search document files on your system";
    if constexpr (Category == vicinae::FileCategory::Archive) return "Search archive files on your system";
    if constexpr (Category == vicinae::FileCategory::Application) return "Search application files on your system";
    return "Search files on your system";
  }

  std::vector<QString> keywords() const override {
    if constexpr (Category == vicinae::FileCategory::Directory) return {"folders", "directories"};
    if constexpr (Category == vicinae::FileCategory::Image) return {"photos", "pictures"};
    if constexpr (Category == vicinae::FileCategory::Video) return {"movies", "clips"};
    if constexpr (Category == vicinae::FileCategory::Audio) return {"music", "songs"};
    if constexpr (Category == vicinae::FileCategory::Document) return {"pdf", "docs"};
    if constexpr (Category == vicinae::FileCategory::Archive) return {"zip", "compressed"};
    if constexpr (Category == vicinae::FileCategory::Application) return {"apps", "programs"};
    return {};
  }

  ImageURL iconUrl() const override { return searchFilesIcon(Category); }
};

} // namespace

class SearchFilesCommand : public BuiltinViewCommand<SearchFilesViewHost> {
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

  void execute(CommandController &controller) const override {
    auto alert = new CallbackAlertWidget;
    auto ctx = controller.context();

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
  QString id() const override { return "files"; }
  QString displayName() const override { return "System files"; }
  QString description() const override { return "Integrate with system files"; }
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
    registerCommand<SearchFilesCategoryCommand<vicinae::FileCategory::Directory>>();
    registerCommand<SearchFilesCategoryCommand<vicinae::FileCategory::Image>>();
    registerCommand<SearchFilesCategoryCommand<vicinae::FileCategory::Video>>();
    registerCommand<SearchFilesCategoryCommand<vicinae::FileCategory::Audio>>();
    registerCommand<SearchFilesCategoryCommand<vicinae::FileCategory::Document>>();
    registerCommand<SearchFilesCategoryCommand<vicinae::FileCategory::Archive>>();
    registerCommand<SearchFilesCategoryCommand<vicinae::FileCategory::Application>>();

    // XXX - we don't really need this anymore, as the indexer now executes full sweeps at a given interval
    // This behavior can be easily replicated by simply removing the `.cache/vicinae/file-indexer` directory.
    // registerCommand<RebuildFileIndexCommand>();
  }

  std::vector<Preference> preferences() const override {
#ifdef Q_OS_LINUX
    auto indexing = Preference::makeCheckbox("autoIndexing");

    indexing.setTitle("Enabled");
    indexing.setDescription(
        "Whether to run the file indexer in the background. When turned off, the indexer process is "
        "stopped entirely and file search becomes unavailable until it is turned back on.");
    indexing.setDefaultValue(true);

    auto paths = Preference::directories("indexingPaths");
    paths.setTitle("Search paths");
    paths.setDescription("Directories that Vicinae will search");
    paths.setDefaultValue(QJsonArray{homeDir().c_str()});

    auto excludedPaths = Preference::directories("excludedIndexingPaths");
    excludedPaths.setTitle("Excluded search paths");
    excludedPaths.setDescription("Directories to exclude from file indexing");
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
