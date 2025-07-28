#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "search-files-view.hpp"
#include "single-view-command-context.hpp"

class SearchFilesCommand : public AbstractViewCommand<SearchFilesView> {
  QString uniqueId() const override { return "file.search"; }
  QString name() const override { return "Search Files"; }
  QString description() const override { return "Search files on your system"; }
  QString extensionId() const override { return "file"; }
  QString commandId() const override { return "search"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("folder").setBackgroundTint(SemanticColor::Red);
  }
  std::vector<Preference> preferences() const override { return {}; }
  void preferenceValuesChanged(const QJsonObject &value) const override {}
};

class FileExtension : public AbstractCommandRepository {
  QString id() const override { return "file"; }
  QString name() const override { return "System files"; }
  QString description() const override { return "Integrate with system files"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("folder").setBackgroundTint(SemanticColor::Red);
  }
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override {
    auto search = std::make_shared<SearchFilesCommand>();

    return {search};
  }
};
