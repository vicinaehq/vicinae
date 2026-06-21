#include "search-files-model.hpp"
#include "utils/file-list-item.hpp"
#include "utils/utils.hpp"

void SearchFilesSection::setFiles(std::vector<std::filesystem::path> files, const QString &sectionName) {
  m_files = std::move(files);
  m_sectionName = sectionName;
  notifyChanged();
}

QString SearchFilesSection::itemId(int i) const { return QString::fromStdString(m_files.at(i).string()); }

QString SearchFilesSection::itemTitle(int i) const {
  return QString::fromStdString(getLastPathComponent(m_files.at(i)));
}

QString SearchFilesSection::itemSubtitle(int i) const { return {}; }

QString SearchFilesSection::itemIconSource(int i) const {
  return imageSourceFor(ImageURL::fileIcon(m_files.at(i)));
}

std::unique_ptr<ActionPanelState> SearchFilesSection::actionPanel(int i) const {
  return FileActions::actionPanel(m_files.at(i), scope().appContext());
}
