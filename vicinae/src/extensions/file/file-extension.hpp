#pragma once
#include "command-database.hpp"
#include "../../ui/image/url.hpp"
#include "common.hpp"
#include "preference.hpp"
#include "search-files-view.hpp"
#include "service-registry.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "single-view-command-context.hpp"
#include "ui/alert/alert.hpp"
#include "utils/utils.hpp"
#include "vicinae.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <ranges>
#include <vector>

class SearchFilesCommand : public BuiltinViewCommand<SearchFilesView> {
  QString id() const override { return "search"; }
  QString name() const override { return "Search Files"; }
  QString description() const override { return "Search files on your system"; }
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

  void execute(const LaunchProps &props, ApplicationContext *ctx) const override {
    auto alert = new CallbackAlertWidget;

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
  FileExtension() {
    registerCommand<SearchFilesCommand>();
    registerCommand<RebuildFileIndexCommand>();
  }

  void preferenceValuesChanged(const QJsonObject &preferences) const override {
    QJsonArray searchPaths = preferences.value("paths").toArray();
    FileService *service = ServiceRegistry::instance()->fileService();

    if (searchPaths.empty()) {
      service->setEntrypoints({{.root = homeDir()}});
      return;
    }

    auto entrypointRange =
      searchPaths | std::views::transform([](const QJsonValue &obj) -> AbstractFileIndexer::Entrypoint {
        return {.root = obj.toString().toStdString()};
      });

    std::vector<AbstractFileIndexer::Entrypoint> entrypoints =
      {entrypointRange.begin(), entrypointRange.end()};

    service->setEntrypoints(entrypoints);
  }
};
