#pragma once
#include "command-database.hpp"
#include "../../ui/image/url.hpp"
#include "search-files-view.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"

class SearchFilesCommand : public BuiltinViewCommand<SearchFilesView> {
  QString id() const override { return "search"; }
  QString name() const override { return "Search Files"; }
  QString description() const override { return "Search files on your system"; }
  QString extensionId() const override { return "file"; }
  QString commandId() const override { return "search"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("magnifying-glass").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  std::vector<Preference> preferences() const override { return {}; }
  void preferenceValuesChanged(const QJsonObject &value) const override {}
};

class FileExtension : public BuiltinCommandRepository {
  QString id() const override { return "file"; }
  QString displayName() const override { return "System files"; }
  QString description() const override { return "Integrate with system files"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("magnifying-glass").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  FileExtension() { registerCommand<SearchFilesCommand>(); }
};
