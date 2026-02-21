#include "search-files-model.hpp"
#include "clipboard-actions.hpp"
#include "misc/file-list-item.hpp"
#include "service-registry.hpp"
#include "utils/utils.hpp"

void SearchFilesModel::setFiles(std::vector<std::filesystem::path> files, const QString &sectionName) {
  m_files = std::move(files);

  setSelectFirstOnReset(false);
  std::vector<SectionInfo> sections;
  if (!m_files.empty())
    sections.push_back({.name = sectionName, .count = static_cast<int>(m_files.size())});
  setSections(sections);
  setSelectFirstOnReset(true);
  refreshActionPanel();
}

const std::filesystem::path &SearchFilesModel::fileAt(int, int item) const { return m_files.at(item); }

QString SearchFilesModel::itemTitle(int s, int i) const {
  return QString::fromStdString(getLastPathComponent(fileAt(s, i)));
}

QString SearchFilesModel::itemIconSource(int s, int i) const {
  return imageSourceFor(ImageURL::fileIcon(fileAt(s, i)));
}

std::unique_ptr<ActionPanelState> SearchFilesModel::createActionPanel(int s, int i) const {
  return FileActions::actionPanel(fileAt(s, i), ctx()->services->appDb());
}

void SearchFilesModel::onItemSelected(int s, int i) { emit fileSelected(fileAt(s, i)); }
