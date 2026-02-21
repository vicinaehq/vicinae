#pragma once
#include "command-list-model.hpp"
#include <filesystem>

class SearchFilesModel : public CommandListModel {
  Q_OBJECT

public:
  using CommandListModel::CommandListModel;

  void setFiles(std::vector<std::filesystem::path> files, const QString &sectionName);

  void setFilter(const QString &) override {}

  const std::filesystem::path &fileAt(int section, int item) const;

protected:
  QString itemTitle(int section, int item) const override;
  QString itemIconSource(int section, int item) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int section, int item) const override;
  void onItemSelected(int section, int item) override;

signals:
  void fileSelected(const std::filesystem::path &path);

private:
  std::vector<std::filesystem::path> m_files;
};
